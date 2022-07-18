#include "StageAPIEditorImpl.h"

#include "EngineUtils.h"
#include "DisplayClusterRootActor.h"
#include "DisplayClusterConfigurationTypes.h"
#include "Components/DisplayClusterICVFXCameraComponent.h"
#include "DisplayCluster/Public/Components/DisplayClusterCameraComponent.h"

#include "Templates/SharedPointer.h"
#include "Toolkits/ToolkitManager.h"
#include "LevelSequence.h"
#include "ILevelSequenceEditorToolkit.h"
#include "LevelSequenceEditorBlueprintLibrary.h"
#include "ISequencer.h"
#include "SequencerSettings.h"

//MULTI USER & TAKE RECORDER INCUDES
#include "TakePreset.h"
#include "TakeMetaData.h"
#include "TakeRecorderSettings.h"
#include "ITakeRecorderModule.h"
#include "Recorder/TakeRecorderBlueprintLibrary.h"
#include "Recorder/TakeRecorderPanel.h"
#include "Recorder/TakeRecorderParameters.h"
#include "IConcertSession.h"
#include "IConcertModule.h"
#include "IConcertClient.h"
#include "IConcertSyncClient.h"
#include "IConcertSyncClientModule.h"
#include "ConcertTakeRecorder/Private/ConcertTakeRecorderSynchronizationCustomization.h"
#include "ConcertSyncCore/Public/ConcertSyncArchives.h"
#include "ConcertTakeRecorder/Private/ConcertTakeRecorderMessages.h"



//NOTE: Internally the API is more procedural than OO in design. This is because it's a "function" based API. For these
//reasons core private vars are static

//Private Functions
static UWorld* s_FindWorldContext();
static bool s_InitAPISurface();
static void s_InitSequencer();

//Private Members
static UWorld* s_GameWorldContext;
static TWeakObjectPtr<ADisplayClusterRootActor> s_DisplayClusterRoot;
static TArray<FString> s_ClusterNodes;
static TArray<FString> s_ViewPorts;
static TMap<FString, FString> s_ViewportToNode;
TWeakPtr<ISequencer> EditorSequencer;

#define API_CHECK_NULL if( !IsAPIReady() && !s_InitAPISurface() ) { return nullptr; }
#define API_CHECK_VOID if( !IsAPIReady() && !s_InitAPISurface() ) { return; }
#define API_CHECK_BOOL if( !IsAPIReady() && !s_InitAPISurface() ) { return false; }
#define API_CHECK_FLOAT if( !IsAPIReady() && !s_InitAPISurface() ) { return 0; }
#define API_CHECK_ROTATOR if( !IsAPIReady() && !s_InitAPISurface() ) { return FRotator(); }
#define API_CHECK_VECTOR if( !IsAPIReady() && !s_InitAPISurface() ) { return FVector(); }
#define API_CHECK_VECTOR4 if( !IsAPIReady() && !s_InitAPISurface() ) { return FVector4(); }
#define API_CHECK_NULL if( !IsAPIReady() && !s_InitAPISurface() ) { return nullptr; }

#define API_CHECK_SEQ_VOID	if (!EditorSequencer.IsValid()) \
								{s_InitSequencer();} \
								if (!EditorSequencer.IsValid()) \
								return;

#define API_CHECK_SEQ_BOOL	if (!EditorSequencer.IsValid()) \
								{ s_InitSequencer(); \
								if (!EditorSequencer.IsValid()) \
								return false;	} \


#define API_CHECK_SEQ_FLOAT	if (!EditorSequencer.IsValid()) \
								{ s_InitSequencer(); \
								if (!EditorSequencer.IsValid()) \
								return 0.0f;	} \


#define API_CHECK_SEQ_INT	if (!EditorSequencer.IsValid()) \
								{ s_InitSequencer(); \
								if (!EditorSequencer.IsValid()) \
								return 0;	} \

#define TEXT_API_TAG "VP Stage API"

//DESTRUCTOR
UStageAPIImpl::~UStageAPIImpl()
{
	s_DisplayClusterRoot.Reset();
	s_ClusterNodes.Reset();
	s_ViewPorts.Reset();
	s_ViewportToNode.Reset();
	s_GameWorldContext = nullptr;
}


/**
 * @brief Helper function to locate the prefered Game World object. 
 * @return an unprotected pointer to the located game world.
 */
UWorld* s_FindWorldContext()
{
	// Prefer PIE Worlds.
	// Fallback to Game Preview Worlds.
	// Ignore all other types (e.g., other preview worlds).

	UWorld* PIE = nullptr;
	UWorld* GamePreview = nullptr;
	UWorld* WorldInEditor = nullptr;

	for (FWorldContext const& Context : GEngine->GetWorldContexts())
	{
		switch (Context.WorldType)
		{
		case EWorldType::PIE:
			PIE = Context.World();
			break;
		case EWorldType::GamePreview:
			GamePreview = Context.World();
			break;
		case EWorldType::Editor:
			WorldInEditor = Context.World();
		}
	}

	if (PIE)
	{
		return PIE;
	}
	else if (GamePreview)
	{
		return GamePreview;
	}
	else if (WorldInEditor)
		return WorldInEditor;

	return nullptr;
}

/**
 * @brief Resets and initalised the API surface. Will attempt to locate a nDisplay Cluster Root Actor in the default world.
 * @return true if the API init was successful
 */
bool s_InitAPISurface()
{
	s_ClusterNodes.Empty();
	s_ViewportToNode.Empty();
	s_GameWorldContext = s_FindWorldContext();
	if (!s_GameWorldContext)
	{
		//TODO: Write error log
		return false;
	}
	
	if (UWorld* World = GEngine->GetWorldFromContextObject(s_GameWorldContext, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AActor> It(World, ADisplayClusterRootActor::StaticClass()); It; ++It)
		{
			s_DisplayClusterRoot = MakeWeakObjectPtr(Cast<ADisplayClusterRootActor>(*It));
		}
	}

	if (s_DisplayClusterRoot.IsValid())
	{
		//Build the tables containing the cluster node and viewport lists.
		for (TPair<FString ,UDisplayClusterConfigurationClusterNode*>& Node : s_DisplayClusterRoot->GetConfigData()->Cluster->Nodes)
		{
			s_ClusterNodes.Add(Node.Key);
			for (TPair<FString, UDisplayClusterConfigurationViewport*>& Viewport : Node.Value->Viewports)
			{
				s_ViewPorts.Add(Viewport.Key);
				s_ViewportToNode.Add(Viewport.Key, Node.Key);
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool UStageAPIImpl::IsAPIReady() const {
	if (s_DisplayClusterRoot.IsValid())
	{
		return true;
	}
	return false;
}

bool UStageAPIImpl::InitAPISurface()
{
	return s_InitAPISurface();
}


//////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE CAMERA FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////
#pragma region "Private Camera Functions"

ACineCameraActor* UStageAPIImpl::GetFrustumCamera_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const
{
	if (!IcvfxComponent)
		return nullptr;

	return IcvfxComponent->CameraSettings.ExternalCameraActor.Get();
	
}
float UStageAPIImpl::GetFrustumFOVMult_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const
{
	if (!IcvfxComponent)
		return 0;
	
	return IcvfxComponent->CameraSettings.BufferRatio;
}
void UStageAPIImpl::SetFrustumFOVMult_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, float FOVMult)
{
	if (!IcvfxComponent)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Set Frustum FOV Mult")), s_DisplayClusterRoot.Get());

	IcvfxComponent->Modify();
	IcvfxComponent->CameraSettings.BufferRatio = FOVMult;

	GEngine->EndTransaction();
	
}
float UStageAPIImpl::GetFrustumExposure_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const
{
	if (!IcvfxComponent)
		return 0;

	return IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.AutoExposureBias;
}
float UStageAPIImpl::GetFrustumAperture_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const
{
	auto FrustumCamera = GetFrustumCamera_ByComponent(IcvfxComponent);

	if (!FrustumCamera) return 0;

	return FrustumCamera->GetCineCameraComponent()->CurrentAperture; 
}
void UStageAPIImpl::SetFrustumExposure_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, float FrustumExposure)
{
	if (!IcvfxComponent)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Set Frustum FOV Mult")), s_DisplayClusterRoot.Get());

	IcvfxComponent->Modify();
	IcvfxComponent->CameraSettings.AllNodesColorGrading.bEnableEntireClusterColorGrading = true;
	IcvfxComponent->CameraSettings.AllNodesColorGrading.bEnableInnerFrustumAllNodesColorGrading = true;
	IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.bOverride_AutoExposureBias = true;
	IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.AutoExposureBias = FrustumExposure;

	GEngine->EndTransaction();
}
void UStageAPIImpl::SetFrustumAperture_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, float FrustumAperture)
{
	auto FrustumCamera = GetFrustumCamera_ByComponent(IcvfxComponent);

	if (!FrustumCamera) return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update frustum aperture")), FrustumCamera);

	FrustumCamera->GetCineCameraComponent()->Modify();
	FrustumCamera->GetCineCameraComponent()->CurrentAperture = FrustumAperture;
	
	GEngine->EndTransaction();
	
}
void UStageAPIImpl::SetFrustumFocalDistance_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent,float FocalDistance)
{
	auto FrustumCamera = GetFrustumCamera_ByComponent(IcvfxComponent);

	if (!FrustumCamera)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update frustum focal distance")), FrustumCamera);

	FrustumCamera->GetCineCameraComponent()->Modify();
	FrustumCamera->GetCineCameraComponent()->FocusSettings.ManualFocusDistance = FocalDistance;
	
	GEngine->EndTransaction();
}

float UStageAPIImpl::GetFrustumFocalDistance_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const
{
	auto FrustumCamera = GetFrustumCamera_ByComponent(IcvfxComponent);

	if (!FrustumCamera)
		return  0;

	return FrustumCamera->GetCineCameraComponent()->FocusSettings.ManualFocusDistance;
}
FRotator UStageAPIImpl::GetFrustumRotation_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const
{
	auto frustumCamera = GetFrustumCamera_ByComponent(IcvfxComponent);
	if (!frustumCamera)
		return  FRotator();

	return frustumCamera->GetRootComponent()->GetRelativeRotation();
}
void UStageAPIImpl::SetFrustumRotation_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent,FRotator NewRotation)
{
	auto FrustumCamera = GetFrustumCamera_ByComponent(IcvfxComponent);

	if (!FrustumCamera)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Set Frustum Rotation")), FrustumCamera);
	FrustumCamera->SetActorRotation(NewRotation,ETeleportType::None);
	FrustumCamera->GetRootComponent()->Modify();
	FrustumCamera->GetRootComponent()->SetRelativeRotation(NewRotation);
	FrustumCamera->Modify();
	GEngine->EndTransaction();
}
void UStageAPIImpl::SetFrustumRotationPreview_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent,FRotator NewRotation)
{
	auto FrustumCamera = GetFrustumCamera_ByComponent(IcvfxComponent);

	if (!FrustumCamera)
		return;

	FrustumCamera->SetActorRotation(NewRotation,ETeleportType::None);
}
FVector UStageAPIImpl::GetFrustumPosition_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const
{
	auto frustumCamera = GetFrustumCamera_ByComponent(IcvfxComponent);
	if (!frustumCamera)
		return  FVector();

	return frustumCamera->GetRootComponent()->GetComponentLocation();
}
void UStageAPIImpl::SetFrustumPosition_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, FVector NewPosition)
{
	auto FrustumCamera = GetFrustumCamera_ByComponent(IcvfxComponent);

	if (!FrustumCamera)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Set Frustum Rotation")), FrustumCamera);
	FrustumCamera->GetRootComponent()->Modify();
	FrustumCamera->GetRootComponent()->SetRelativeLocation(NewPosition);
	FrustumCamera->Modify();
	GEngine->EndTransaction();
}
void UStageAPIImpl::SetFrustumPositionPreview_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, FVector NewPosition)
{
	auto FrustumCamera = GetFrustumCamera_ByComponent(IcvfxComponent);

	if (!FrustumCamera)
		return;

	FrustumCamera->GetRootComponent()->SetRelativeLocation(NewPosition);
}

#pragma endregion


//////////////////////////////////////////////////////////////////////////////////////////////
// Camera API (Main & Camera B)
//////////////////////////////////////////////////////////////////////////////////////////////
#pragma region "Camera API"

bool UStageAPIImpl::CameraBActive() const
{
	return (!(GetIcvfxCameraComponentA() == GetIcvfxCameraComponentB()));
}

ACineCameraActor* UStageAPIImpl::GetFrustumCamera() const
{
	API_CHECK_NULL
	return GetFrustumCamera_ByComponent(GetIcvfxCameraComponentA());
}
ACineCameraActor* UStageAPIImpl::GetFrustumCameraB() const
{
	API_CHECK_NULL
	return GetFrustumCamera_ByComponent(GetIcvfxCameraComponentB());
}

float UStageAPIImpl::GetFrustumFOVMult() const
{
	API_CHECK_FLOAT
	return  GetFrustumFOVMult_ByComponent(GetIcvfxCameraComponentA());

}
float UStageAPIImpl::GetFrustumFOVMultB() const
{
	API_CHECK_FLOAT
	return  GetFrustumFOVMult_ByComponent(GetIcvfxCameraComponentB());
}

void UStageAPIImpl::SetFrustumFOVMult(float FOVMult)
{
	API_CHECK_VOID
	SetFrustumFOVMult_ByComponent(GetIcvfxCameraComponentA(),FOVMult);
}
void UStageAPIImpl::SetFrustumFOVMultB(float FOVMult)
{
	API_CHECK_VOID
	SetFrustumFOVMult_ByComponent(GetIcvfxCameraComponentB(),FOVMult);
}

float UStageAPIImpl::GetFrustumExposure() const
{
	API_CHECK_FLOAT
	return  GetFrustumExposure_ByComponent(GetIcvfxCameraComponentA());
}
float UStageAPIImpl::GetFrustumExposureB() const
{
	API_CHECK_FLOAT
	return  GetFrustumExposure_ByComponent(GetIcvfxCameraComponent());
}

float UStageAPIImpl::GetFrustumAperture() const
{
	API_CHECK_FLOAT
	return GetFrustumAperture_ByComponent(GetIcvfxCameraComponentA());
}
float UStageAPIImpl::GetFrustumApertureB() const
{
	API_CHECK_FLOAT
	return GetFrustumAperture_ByComponent(GetIcvfxCameraComponentB());
}

void UStageAPIImpl::SetFrustumExposure(float FrustumExposure)
{
	API_CHECK_VOID
	SetFrustumExposure_ByComponent(GetIcvfxCameraComponentA(), FrustumExposure);
}
void UStageAPIImpl::SetFrustumExposureB(float FrustumExposure)
{
	API_CHECK_VOID
	SetFrustumExposure_ByComponent(GetIcvfxCameraComponentB(), FrustumExposure);
}

void UStageAPIImpl::SetFrustumAperture(float FrustumAperture)
{
	API_CHECK_VOID
	SetFrustumAperture_ByComponent(GetIcvfxCameraComponentA(), FrustumAperture);
}
void UStageAPIImpl::SetFrustumApertureB(float FrustumAperture)
{
	API_CHECK_VOID
	SetFrustumAperture_ByComponent(GetIcvfxCameraComponentB(), FrustumAperture);
}

void UStageAPIImpl::SetFrustumFocalDistance(float FocalDistance)
{
	API_CHECK_VOID
	SetFrustumFocalDistance_ByComponent(GetIcvfxCameraComponentA(), FocalDistance);

	//Call any listeners to focal distance changes
	/*
	for (auto Listner : Listeners_FrustumFocalDistance)
	{
		Listner->ExecuteIfBound(FocalDistance);
	}*/
}
void UStageAPIImpl::SetFrustumFocalDistanceB(float FocalDistance)
{
	API_CHECK_VOID
	SetFrustumFocalDistance_ByComponent(GetIcvfxCameraComponentB(), FocalDistance);

	//TODO: Implement focal dist listner for Camera B
/*
	//Call any listeners to focal distance changes
	for (auto Listner : Listeners_FrustumFocalDistance)
	{
		Listner->ExecuteIfBound(FocalDistance);
	}
*/
}

float UStageAPIImpl::GetFrustumFocalDistance() const
{
	API_CHECK_FLOAT
	return GetFrustumFocalDistance_ByComponent(GetIcvfxCameraComponentA());
}
float UStageAPIImpl::GetFrustumFocalDistanceB() const
{
	API_CHECK_FLOAT
	return GetFrustumFocalDistance_ByComponent(GetIcvfxCameraComponentB());
}


FRotator UStageAPIImpl::GetFrustumRotation() const
{
	API_CHECK_ROTATOR
	return  GetFrustumRotation_ByComponent(GetIcvfxCameraComponentA());
}
FRotator UStageAPIImpl::GetFrustumRotationB() const
{
	API_CHECK_ROTATOR
	return  GetFrustumRotation_ByComponent(GetIcvfxCameraComponentB());
}

void UStageAPIImpl::SetFrustumRotation(FRotator NewRotation)
{
	API_CHECK_VOID
	SetFrustumRotation_ByComponent(GetIcvfxCameraComponentA(), NewRotation);
}
void UStageAPIImpl::SetFrustumRotationB(FRotator NewRotation)
{
	API_CHECK_VOID
	SetFrustumRotation_ByComponent(GetIcvfxCameraComponentB(), NewRotation);
}

void UStageAPIImpl::SetFrustumRotationPreview(FRotator NewRotation)
{
	API_CHECK_VOID
	SetFrustumRotationPreview_ByComponent(GetIcvfxCameraComponentA(),NewRotation);
}
void UStageAPIImpl::SetFrustumRotationPreviewB(FRotator NewRotation)
{
	API_CHECK_VOID
	SetFrustumRotationPreview_ByComponent(GetIcvfxCameraComponentB(),NewRotation);
}

FVector UStageAPIImpl::GetFrustumPosition() const
{
	API_CHECK_VECTOR
	return GetFrustumPosition_ByComponent(GetIcvfxCameraComponentA());
}
FVector UStageAPIImpl::GetFrustumPositionB() const
{
	API_CHECK_VECTOR
	return GetFrustumPosition_ByComponent(GetIcvfxCameraComponentB());
}

void UStageAPIImpl::SetFrustumPosition(FVector NewPosition)
{
	API_CHECK_VOID
	SetFrustumPosition_ByComponent(GetIcvfxCameraComponentA(), NewPosition);
}
void UStageAPIImpl::SetFrustumPositionB(FVector NewPosition)
{
	API_CHECK_VOID
	SetFrustumPosition_ByComponent(GetIcvfxCameraComponentB(), NewPosition);
}

void UStageAPIImpl::SetFrustumPositionPreview(FVector NewPosition)
{
	API_CHECK_VOID
	SetFrustumPositionPreview_ByComponent(GetIcvfxCameraComponentA(), NewPosition);
}

//TODO: Update this function to the same layout as other ICVFX calls
float UStageAPIImpl::GetFrustumRenderRatio() const
{
	API_CHECK_FLOAT

	auto ICVFXCamera = GetIcvfxCameraComponent();

	if (!ICVFXCamera)
		return 0;

	//return ICVFXCamera->CameraSettings.BufferRatio;
	return ICVFXCamera->CameraSettings.RenderSettings.AdvancedRenderSettings.RenderTargetRatio;
}

//TODO: Update this function to the same layout as other ICVFX calls
void UStageAPIImpl::SetFrustumRenderRatio(float ScreenPercentage)
{
	API_CHECK_VOID

	auto ICVFXCamera = GetIcvfxCameraComponent();

	if (!ICVFXCamera)
		return;

	GEngine->BeginTransaction(*FString(TEXT("VP Stage API Editor API")), FText::FromString(TEXT("Set ICVFX Screen Percentage")), s_DisplayClusterRoot.Get());

	ICVFXCamera->Modify();
	//ICVFXCamera->CameraSettings.BufferRatio = ScreenPercentage;
	ICVFXCamera->CameraSettings.RenderSettings.AdvancedRenderSettings.RenderTargetRatio = ScreenPercentage;

	GEngine->EndTransaction();
	
}

void UStageAPIImpl::SetFrustumPositionPreviewB(FVector NewPosition)
{
	API_CHECK_VOID
	SetFrustumPositionPreview_ByComponent(GetIcvfxCameraComponentB(), NewPosition);
}


#pragma endregion 

#pragma region "API Calls"


//////////////////////////////////////////////////////////////////////////////////////////////
// Stage Calls
//////////////////////////////////////////////////////////////////////////////////////////////
#pragma region "Stage Calls"


#pragma endregion //END Stage Calls


//////////////////////////////////////////////////////////////////////////////////////////////
// nDisplay API
//////////////////////////////////////////////////////////////////////////////////////////////
#pragma region "nDisplay API"

////** Gets the Display Cluster Root Actor. */
ADisplayClusterRootActor* UStageAPIImpl::GetDisplayClusterRoot() const
{
	API_CHECK_NULL

	return s_DisplayClusterRoot.Get();
}

void UStageAPIImpl::DisableInnerFrustum()
{
	SetInnerFrustumState(false);
}

void UStageAPIImpl::EnableInnerFrustum()
{
	SetInnerFrustumState(true);
}

bool UStageAPIImpl::GetInnerFrustumStatus() const
{
	API_CHECK_BOOL

	return s_DisplayClusterRoot->GetConfigData()->StageSettings.bEnableInnerFrustums;
}

void UStageAPIImpl::SetInnerFrustumState(bool NewInnerFrustumState)
{
	API_CHECK_VOID

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Enable/disable inner frustums")), s_DisplayClusterRoot.Get());

	s_DisplayClusterRoot->GetConfigData()->Modify();
	s_DisplayClusterRoot->GetConfigData()->StageSettings.bEnableInnerFrustums = NewInnerFrustumState;

	GEngine->EndTransaction();
}


UDisplayClusterICVFXCameraComponent* UStageAPIImpl::GetIcvfxCameraComponent() const
{
	return GetIcvfxCameraComponentA();
}

UDisplayClusterICVFXCameraComponent* UStageAPIImpl::GetIcvfxCameraComponentA() const
{
	API_CHECK_NULL

	TArray<UDisplayClusterICVFXCameraComponent*> AvaliablleICVFXComponents;
	s_DisplayClusterRoot->GetComponents<UDisplayClusterICVFXCameraComponent>(AvaliablleICVFXComponents,false);
	
	UDisplayClusterICVFXCameraComponent* ReturnICVFXComponent = nullptr;

	//We grab the first component if avaliable or the one with the highest priority order
	for (auto ICVFXComponent : AvaliablleICVFXComponents) 
	{
		if (ICVFXComponent->CameraSettings.bEnable)
		{
			if (!ReturnICVFXComponent)
				ReturnICVFXComponent = ICVFXComponent;
			else
			{
				if(ICVFXComponent->CameraSettings.RenderSettings.RenderOrder > ReturnICVFXComponent->CameraSettings.RenderSettings.RenderOrder)
					ReturnICVFXComponent = ICVFXComponent;
			}
		}
	}
	return ReturnICVFXComponent;
}

UDisplayClusterICVFXCameraComponent* UStageAPIImpl::GetIcvfxCameraComponentB() const
{
	API_CHECK_NULL

	TArray<UDisplayClusterICVFXCameraComponent*> AvaliablleICVFXComponents;
	s_DisplayClusterRoot->GetComponents<UDisplayClusterICVFXCameraComponent>(AvaliablleICVFXComponents,false);
	
	UDisplayClusterICVFXCameraComponent* ReturnICVFXComponent = nullptr;

	//We grab the first component if avaliable or the one with the lowest priority order
	for (auto ICVFXComponent : AvaliablleICVFXComponents) 
	{
		if (ICVFXComponent->CameraSettings.bEnable)
		{
			if (!ReturnICVFXComponent)
				ReturnICVFXComponent = ICVFXComponent;
			else
			{
				if(ICVFXComponent->CameraSettings.RenderSettings.RenderOrder < ReturnICVFXComponent->CameraSettings.RenderSettings.RenderOrder)
					ReturnICVFXComponent = ICVFXComponent;
			}
		}
	}
	return ReturnICVFXComponent;
}

TArray<FString> UStageAPIImpl::GetViewportNames() const
{
	return s_ViewPorts;
}

float UStageAPIImpl::GetGlobalScreenPercentage() const
{
	API_CHECK_FLOAT

	auto ClusterConfiguration = GetDisplayClusterRoot()->GetConfigData();

	if (!ClusterConfiguration) return 0;

	return  ClusterConfiguration->RenderFrameSettings.ClusterICVFXOuterViewportBufferRatioMult;
	
}

void UStageAPIImpl::SetGlobalScreenPercentage(float NewGlobalScreenPercentage)
{
	API_CHECK_VOID

	auto ClusterConfiguration = GetDisplayClusterRoot()->GetConfigData();

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Set Global Screen Percentage")), s_DisplayClusterRoot.Get());

	ClusterConfiguration->Modify();
	ClusterConfiguration->RenderFrameSettings.ClusterICVFXOuterViewportBufferRatioMult = NewGlobalScreenPercentage;
	
	GEngine->EndTransaction();
}

void UStageAPIImpl::SetStageLocation(FVector StagePosition, FRotator StageRotation)
{
	API_CHECK_VOID

	//Look to see if the DCR is has a parent, which is going to be the stage root
	auto StageRoot = GetDisplayClusterRoot()->GetAttachParentActor();

	if(StageRoot)
	{
		GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update Stage Location")), StageRoot);
		StageRoot->Modify();
		StageRoot->SetActorLocation(StagePosition);
		StageRoot->SetActorRotation(StageRotation);
		GEngine->EndTransaction();
	}
	else
	{
		GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update Stage Location")), s_DisplayClusterRoot.Get());
		s_DisplayClusterRoot->Modify();
		s_DisplayClusterRoot->SetActorLocation(StagePosition);
		s_DisplayClusterRoot->SetActorRotation(StageRotation);
		GEngine->EndTransaction();
	}

}

void UStageAPIImpl::SetStageLocalPosition(FVector StagePosition)
{
	API_CHECK_VOID

//Look to see if the DCR is has a parent, which is going to be the stage root
auto StageRoot = GetDisplayClusterRoot()->GetAttachParentActor();

	if(StageRoot)
	{
		GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update Stage Location")), StageRoot);
		StageRoot->Modify();
		StageRoot->SetActorLocation(StagePosition);
		GEngine->EndTransaction();
	}
	else
	{
		GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update Stage Location")), s_DisplayClusterRoot.Get());
		s_DisplayClusterRoot->Modify();
		s_DisplayClusterRoot->SetActorLocation(StagePosition);
		GEngine->EndTransaction();
	}
}

void UStageAPIImpl::AddStageLocalOffset(FVector DeltaLocation)
{
	API_CHECK_VOID

//Look to see if the DCR is has a parent, which is going to be the stage root
	auto StageRoot = GetDisplayClusterRoot()->GetAttachParentActor();
	FHitResult HitResult;
	
	if(StageRoot)
	{
		GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update Stage Location")), StageRoot);
		StageRoot->Modify();
		StageRoot->K2_AddActorLocalOffset(DeltaLocation,false,HitResult,true);
		GEngine->EndTransaction();
	}
	else
	{
		GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update Stage Location")), s_DisplayClusterRoot.Get());
		s_DisplayClusterRoot->Modify();
		s_DisplayClusterRoot->K2_AddActorLocalOffset(DeltaLocation,false,HitResult,true);
		GEngine->EndTransaction();
	}
}

void UStageAPIImpl::SetStageLocalRotation(FRotator StageRotation)
{
	API_CHECK_VOID

	//Look to see if the DCR is has a parent, which is going to be the stage root
	auto StageRoot = GetDisplayClusterRoot()->GetAttachParentActor();

	FHitResult HitResult;

	if(StageRoot)
	{
		GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update Stage Location")), StageRoot);
		StageRoot->Modify();
		StageRoot->SetActorRotation(StageRotation);
		StageRoot->GetRootComponent()->K2_SetWorldRotation(StageRotation,false,HitResult,true);
		GEngine->EndTransaction();
	}
	else
	{
		GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update Stage Location")), s_DisplayClusterRoot.Get());
		s_DisplayClusterRoot->Modify();
		s_DisplayClusterRoot->GetRootComponent()->K2_SetWorldRotation(StageRotation,false,HitResult,true);
		GEngine->EndTransaction();
	}
}

FRotator UStageAPIImpl::GetStageLocalRotation()
{
	API_CHECK_ROTATOR

	//Look to see if the DCR is has a parent, which is going to be the stage root
auto const StageRoot = GetDisplayClusterRoot()->GetAttachParentActor();

	if(StageRoot)
	{
		return  StageRoot->K2_GetActorRotation();
	}
	else
	{
		return s_DisplayClusterRoot->K2_GetActorRotation();
	}
}

FRotator UStageAPIImpl::GetStageWorldRotation()
{
	API_CHECK_ROTATOR

//Look to see if the DCR is has a parent, which is going to be the stage root
auto const StageRoot = GetDisplayClusterRoot()->GetAttachParentActor();

	if(StageRoot)
	{
		return  StageRoot->GetRootComponent()->K2_GetComponentRotation();
	}
	else
	{
		return s_DisplayClusterRoot->GetRootComponent()->K2_GetComponentRotation();
	}
}

void UStageAPIImpl::SetStageWorldRotation(FRotator StageRotation)
{
	API_CHECK_VOID

	FHitResult HitResult;

//Look to see if the DCR is has a parent, which is going to be the stage root
auto const StageRoot = GetDisplayClusterRoot()->GetAttachParentActor();

	if(StageRoot)
	{
		return  StageRoot->GetRootComponent()->K2_SetWorldRotation(StageRotation,false, HitResult, true);
	}
	else
	{
		return s_DisplayClusterRoot->GetRootComponent()->K2_SetWorldRotation(StageRotation,false, HitResult, true);
	}
}

FVector UStageAPIImpl::GetStagePosition()
{
	API_CHECK_VECTOR

	auto const StageRoot = GetDisplayClusterRoot()->GetAttachParentActor();

	if(StageRoot)
	{
		return  StageRoot->K2_GetActorLocation();
	}
	else
	{
		return s_DisplayClusterRoot->K2_GetActorLocation();
	}
	
}

FVector UStageAPIImpl::GetDefaultViewPosition() const
{
	API_CHECK_VECTOR
	
	FObjectProperty* DefaultViewPoint_Property = CastField<FObjectProperty>(s_DisplayClusterRoot->GetClass()->FindPropertyByName("DefaultViewPoint"));
	
	if (DefaultViewPoint_Property)
	{
		UDisplayClusterCameraComponent* DefaultViewPoint = Cast<UDisplayClusterCameraComponent>(DefaultViewPoint_Property->GetObjectPropertyValue_InContainer(s_DisplayClusterRoot.Get()));
		if (DefaultViewPoint)
		{
			return DefaultViewPoint->GetRelativeLocation();
			//return DefaultViewPoint->GetRelativeTransform().GetLocation();
		}
	}
	return FVector();
}

void UStageAPIImpl::SetDefaultViewPosition(FVector NewPosition)
{
	API_CHECK_VOID

	FObjectProperty* DefaultViewPoint_Property = CastField<FObjectProperty>(s_DisplayClusterRoot->GetClass()->FindPropertyByName("DefaultViewPoint"));
	
	if (DefaultViewPoint_Property)
	{
		UDisplayClusterCameraComponent* DefaultViewPoint = Cast<UDisplayClusterCameraComponent>(DefaultViewPoint_Property->GetObjectPropertyValue_InContainer(s_DisplayClusterRoot.Get()));
		if (DefaultViewPoint)
		{
			GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update Default View Location")), s_DisplayClusterRoot.Get());
			DefaultViewPoint->Modify();
			DefaultViewPoint->SetRelativeLocation(NewPosition);
			//DefaultViewPoint->GetRelativeTransform().SetLocation(NewPosition);
			GEngine->EndTransaction();
		}
	}
}




#pragma endregion //END nDisplay API



#pragma region "Image & Color API"


//////////////////////////////////////////////////////////////////////////////////////////////
// Image & Color API
//////////////////////////////////////////////////////////////////////////////////////////////

float UStageAPIImpl::GetStageExposure() const
{
	API_CHECK_FLOAT

	return s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.AutoExposureBias;
	
}

void UStageAPIImpl::SetStageExposure(float ExposureCompensation)
{
	API_CHECK_VOID

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update stage exposure")), s_DisplayClusterRoot.Get());

	s_DisplayClusterRoot->GetConfigData()->Modify();
	s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.bEnableEntireClusterColorGrading = true;
	s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.bOverride_AutoExposureBias = true;
	s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.AutoExposureBias = ExposureCompensation;
	
	GEngine->EndTransaction();
}

void UStageAPIImpl::DisableStageExposure()
{
	API_CHECK_VOID

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update stage exposure")), s_DisplayClusterRoot.Get());

	s_DisplayClusterRoot->GetConfigData()->Modify();
	s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.bOverride_AutoExposureBias = false;
	
	GEngine->EndTransaction();

	
}

void UStageAPIImpl::SetChromakeyStatus(bool ChromakeyEnabled)
{
	if (ChromakeyEnabled)
		EnableChromakey();
	else
		DisableChromakey();
}

void UStageAPIImpl::DisableChromakey()
{
	API_CHECK_VOID

	auto IcVFXComponent = GetIcvfxCameraComponent();
	if (!IcVFXComponent)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Disable chromakey")), s_DisplayClusterRoot.Get());
	s_DisplayClusterRoot->Modify();
	IcVFXComponent->Modify();
	IcVFXComponent->CameraSettings.Chromakey.bEnable = false;

	GEngine->EndTransaction();
	
}

void UStageAPIImpl::EnableChromakey()
{
	API_CHECK_VOID
	
	auto IcVFXComponent = GetIcvfxCameraComponent();
	if (!IcVFXComponent)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Enable chromakey")), s_DisplayClusterRoot.Get());
	s_DisplayClusterRoot->Modify();
	IcVFXComponent->Modify();
	IcVFXComponent->CameraSettings.Chromakey.bEnable = true;

	GEngine->EndTransaction();
}

bool UStageAPIImpl::GetChromakeyStatus() const
{
	API_CHECK_BOOL

	auto IcVFXComponent = GetIcvfxCameraComponent();
	if (!IcVFXComponent)
		return 0;

	return IcVFXComponent->CameraSettings.Chromakey.bEnable;
}

FVector4 UStageAPIImpl::GetClusterPP_GlobalSaturation() const
{
	API_CHECK_VECTOR4
	return s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.Saturation;
}

//CLUSTER GRADE FUNCTIONS

void UStageAPIImpl::SetClusterPP_GlobalSaturation(FVector4 NewSaturation)
{
	API_CHECK_VOID

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update post process global saturation")), s_DisplayClusterRoot.Get());

	s_DisplayClusterRoot->GetConfigData()->Modify();
	if (NewSaturation.X == 0 && NewSaturation.Y == 0 && NewSaturation.Z == 0)
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.bOverride_Saturation = false;
	}
	else
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.bEnableEntireClusterColorGrading = true;
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.bOverride_Saturation = true;
	}
	
	s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.Saturation = NewSaturation;
	
	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetClusterPP_GlobalContrast() const
{
	API_CHECK_VECTOR4
	return s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.Contrast;
}

void UStageAPIImpl::SetClusterPP_GlobalContrast(FVector4 NewContrast)
{
	API_CHECK_VOID

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update post process global saturation")), s_DisplayClusterRoot.Get());

	s_DisplayClusterRoot->GetConfigData()->Modify();
	if (NewContrast.X == 0 && NewContrast.Y == 0 && NewContrast.Z == 0)
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.bOverride_Contrast = false;
	}
	else
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.bEnableEntireClusterColorGrading = true;
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.bOverride_Contrast = true;
	}
	
	s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.Contrast = NewContrast;
	
	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetClusterPP_GlobalGamma() const
{
	API_CHECK_VECTOR4
	return s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.Gamma;
}

void UStageAPIImpl::SetClusterPP_GlobalGamma(FVector4 NewGamma)
{
	API_CHECK_VOID

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update post process global saturation")), s_DisplayClusterRoot.Get());

	s_DisplayClusterRoot->GetConfigData()->Modify();
	if (NewGamma.X == 0 && NewGamma.Y == 0 && NewGamma.Z == 0)
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.bOverride_Gamma = false;
	}
	else
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.bEnableEntireClusterColorGrading = true;
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.bOverride_Gamma = true;
	}
	
	s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.Gamma = NewGamma;
	
	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetClusterPP_GlobalGain() const
{
	API_CHECK_VECTOR4
	return s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.Gain;
}

void UStageAPIImpl::SetClusterPP_GlobalGain(FVector4 NewGain)
{
	API_CHECK_VOID

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update post process global saturation")), s_DisplayClusterRoot.Get());

	s_DisplayClusterRoot->GetConfigData()->Modify();
	if (NewGain.X == 0 && NewGain.Y == 0 && NewGain.Z == 0)
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.bOverride_Gain = false;
	}
	else
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.bEnableEntireClusterColorGrading = true;
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.bOverride_Gain = true;
	}
	
	s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.Gain = NewGain;
	
	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetClusterPP_GlobalOffset() const
{
	API_CHECK_VECTOR4
	return s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.Offset;
}

void UStageAPIImpl::SetClusterPP_GlobalOffset(FVector4 NewOffset)
{
	API_CHECK_VOID

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update post process global saturation")), s_DisplayClusterRoot.Get());

	s_DisplayClusterRoot->GetConfigData()->Modify();
	if (NewOffset.X == 0 && NewOffset.Y == 0 && NewOffset.Z == 0)
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.bOverride_Offset = false;
	}
	else
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.bEnableEntireClusterColorGrading = true;
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.bOverride_Offset = true;
	}
	
	s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Global.Offset = NewOffset;
	
	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetClusterPP_ShadowsGain() const
{
	API_CHECK_VECTOR4
	return s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Shadows.Gain;
}


void UStageAPIImpl::SetClusterPP_ShadowsGain(FVector4 NewShadowsGain)
{
	API_CHECK_VOID

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update post process global saturation")), s_DisplayClusterRoot.Get());

	s_DisplayClusterRoot->GetConfigData()->Modify();
	if (NewShadowsGain.X == 0 && NewShadowsGain.Y == 0 && NewShadowsGain.Z == 0)
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Shadows.bOverride_Gain = false;
	}
	else
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.bEnableEntireClusterColorGrading = true;
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Shadows.bOverride_Gain = true;
	}
	
	s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Shadows.Gain = NewShadowsGain;
	
	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetClusterPP_MidsGain() const
{
	API_CHECK_VECTOR4
	return s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Midtones.Gain;
}

void UStageAPIImpl::SetClusterPP_MidsGain(FVector4 NewMidsGain)
{
	API_CHECK_VOID

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update post process global saturation")), s_DisplayClusterRoot.Get());

	s_DisplayClusterRoot->GetConfigData()->Modify();
	if (NewMidsGain.X == 0 && NewMidsGain.Y == 0 && NewMidsGain.Z == 0)
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Midtones.bOverride_Gain = false;
	}
	else
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.bEnableEntireClusterColorGrading = true;
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Midtones.bOverride_Gain = true;
	}
	
	s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Midtones.Gain = NewMidsGain;
	
	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetClusterPP_HighlightsGain() const
{
	API_CHECK_VECTOR4
	return s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Highlights.Gain;
}

void UStageAPIImpl::SetClusterPP_HighlightsGain(FVector4 NewHighlightsGain)
{
	API_CHECK_VOID

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update post process global saturation")), s_DisplayClusterRoot.Get());

	s_DisplayClusterRoot->GetConfigData()->Modify();
	if (NewHighlightsGain.X == 0 && NewHighlightsGain.Y == 0 && NewHighlightsGain.Z == 0)
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Highlights.bOverride_Gain = false;
	}
	else
	{
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.bEnableEntireClusterColorGrading = true;
		s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Highlights.bOverride_Gain = true;
	}
	
	s_DisplayClusterRoot->GetConfigData()->StageSettings.EntireClusterColorGrading.ColorGradingSettings.Highlights.Gain = NewHighlightsGain;
	
	GEngine->EndTransaction();
}

//FRUSTUM GRADE FUNCTIONS

FVector4 UStageAPIImpl::GetFrustumPP_GlobalSaturation() const
{
	API_CHECK_VECTOR4
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return FVector4();

	return IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.Saturation;
}

void UStageAPIImpl::SetFrustumPP_GlobalSaturation(FVector4 NewSaturation)
{
	API_CHECK_VOID
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update ICVFX Color Grade")), s_DisplayClusterRoot.Get());
	IcvfxComponent->Modify();

	if (NewSaturation.X == 0 && NewSaturation.Y == 0 && NewSaturation.Z == 0)
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.bOverride_Saturation = false;
	}
	else
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.bEnableEntireClusterColorGrading = true;
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.bOverride_Saturation = true;
		
	}
	IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.Saturation = NewSaturation;

	GEngine->EndTransaction();
}


FVector4 UStageAPIImpl::GetFrustumPP_GlobalContrast() const
{
	API_CHECK_VECTOR4
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return FVector4();

	return IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.Contrast;
}

void UStageAPIImpl::SetFrustumPP_GlobalContrast(FVector4 NewContrast)
{
	API_CHECK_VOID
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update ICVFX Color Grade")), s_DisplayClusterRoot.Get());
	IcvfxComponent->Modify();

	if (NewContrast.X == 0 && NewContrast.Y == 0 && NewContrast.Z == 0)
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.bOverride_Contrast = false;
	}
	else
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.bEnableEntireClusterColorGrading = true;
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.bOverride_Contrast = true;
		
	}
	IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.Contrast = NewContrast;

	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetFrustumPP_GlobalGamma() const
{
	API_CHECK_VECTOR4
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return FVector4();

	return IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.Gamma;
}

void UStageAPIImpl::SetFrustumPP_GlobalGamma(FVector4 NewGamma)
{
	API_CHECK_VOID
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update ICVFX Color Grade")), s_DisplayClusterRoot.Get());
	IcvfxComponent->Modify();

	if (NewGamma.X == 0 && NewGamma.Y == 0 && NewGamma.Z == 0)
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.bOverride_Gain = false;
	}
	else
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.bEnableEntireClusterColorGrading = true;
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.bOverride_Gain = true;
		
	}
	IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.Gamma = NewGamma;

	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetFrustumPP_GlobalGain() const
{
	API_CHECK_VECTOR4
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return FVector4();

	return IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.Gain;
}

void UStageAPIImpl::SetFrustumPP_GlobalGain(FVector4 NewGain)
{
	API_CHECK_VOID
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update ICVFX Color Grade")), s_DisplayClusterRoot.Get());
	IcvfxComponent->Modify();

	if (NewGain.X == 0 && NewGain.Y == 0 && NewGain.Z == 0)
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.bOverride_Gain = false;
	}
	else
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.bEnableEntireClusterColorGrading = true;
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.bOverride_Gain = true;
		
	}
	IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.Gain = NewGain;

	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetFrustumPP_GlobalOffset() const
{
	API_CHECK_VECTOR4
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return FVector4();

	return IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.Offset;
}

void UStageAPIImpl::SetFrustumPP_GlobalOffset(FVector4 NewOffset)
{
	API_CHECK_VOID
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update ICVFX Color Grade")), s_DisplayClusterRoot.Get());
	IcvfxComponent->Modify();

	if (NewOffset.X == 0 && NewOffset.Y == 0 && NewOffset.Z == 0)
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.bOverride_Offset = false;
	}
	else
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.bEnableEntireClusterColorGrading = true;
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.bOverride_Offset = true;
		
	}
	IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Global.Offset = NewOffset;

	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetFrustumPP_ShadowsGain() const
{
	API_CHECK_VECTOR4
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return FVector4();

	return IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Shadows.Gain;
}

void UStageAPIImpl::SetFrustumPP_ShadowsGain(FVector4 NewShadowsGain)
{
	API_CHECK_VOID
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update ICVFX Color Grade")), s_DisplayClusterRoot.Get());
	IcvfxComponent->Modify();

	if (NewShadowsGain.X == 0 && NewShadowsGain.Y == 0 && NewShadowsGain.Z == 0)
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Shadows.bOverride_Gain = false;
	}
	else
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.bEnableEntireClusterColorGrading = true;
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Shadows.bOverride_Gain = true;
		
	}
	IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Shadows.Gain = NewShadowsGain;

	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetFrustumPP_MidsGain() const
{
	API_CHECK_VECTOR4
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return FVector4();

	return IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Midtones.Gain;
}

void UStageAPIImpl::SetFrustumPP_MidsGain(FVector4 NewMidsGain)
{
	API_CHECK_VOID
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update ICVFX Color Grade")), s_DisplayClusterRoot.Get());
	IcvfxComponent->Modify();

	if (NewMidsGain.X == 0 && NewMidsGain.Y == 0 && NewMidsGain.Z == 0)
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Midtones.bOverride_Gain = false;
	}
	else
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.bEnableEntireClusterColorGrading = true;
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Midtones.bOverride_Gain = true;
		
	}
	IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Midtones.Gain = NewMidsGain;

	GEngine->EndTransaction();
}

FVector4 UStageAPIImpl::GetFrustumPP_HighlightsGain() const
{
	API_CHECK_VECTOR4
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return FVector4();

	return IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Highlights.Gain;
}

void UStageAPIImpl::SetFrustumPP_HighlightsGain(FVector4 NewHighlightsGain)
{
	API_CHECK_VOID
	
	auto const IcvfxComponent = GetIcvfxCameraComponentA();

	if (!IcvfxComponent)
		return;

	GEngine->BeginTransaction(*FString(TEXT(TEXT_API_TAG)), FText::FromString(TEXT("Update ICVFX Color Grade")), s_DisplayClusterRoot.Get());
	IcvfxComponent->Modify();

	if (NewHighlightsGain.X == 0 && NewHighlightsGain.Y == 0 && NewHighlightsGain.Z == 0)
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Highlights.bOverride_Gain = false;
	}
	else
	{
		IcvfxComponent->CameraSettings.AllNodesColorGrading.bEnableEntireClusterColorGrading = true;
		IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Highlights.bOverride_Gain = true;
		
	}
	IcvfxComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Highlights.Gain = NewHighlightsGain;

	GEngine->EndTransaction();
}



#pragma  endregion

#pragma region "Sequencer Controls"


void s_InitSequencer()
{

	//TODO: This code is dependent on there being an already open sequence in order to find the Sequencer. We should be able to find the ToolKit directly
	auto OpenAssets = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->GetAllEditedAssets();
	for (UObject* item : OpenAssets)
	{
		auto Manager = FToolkitManager::Get().FindEditorForAsset(item);
		auto ManagerObject = Manager.Get();
		ILevelSequenceEditorToolkit* LevelSequenceEditorToolkit = (ILevelSequenceEditorToolkit*)(ManagerObject);
		if (LevelSequenceEditorToolkit)
		{
			auto SequenceToolKitSharedRef = LevelSequenceEditorToolkit->GetSequencer().ToSharedRef();
			EditorSequencer = TWeakPtr<ISequencer>(SequenceToolKitSharedRef);

			return;
		}
	}
}

	//SEQUENCER CONTROLS


float UStageAPIImpl::GetSequencerTime() const
{
	API_CHECK_SEQ_FLOAT

	auto CurrentPlayheadTime = EditorSequencer.Pin()->GetGlobalTime();
	return CurrentPlayheadTime.AsSeconds();
}

void UStageAPIImpl::SetSequencerTime(float PlayHeadInSeconds)
{
	API_CHECK_SEQ_VOID
	
	FQualifiedFrameTime const CurrentPlayheadTime = EditorSequencer.Pin()->GetGlobalTime();
	int const PlayHeadPosition = CurrentPlayheadTime.Rate.Numerator * PlayHeadInSeconds;
	FFrameTime const NewPlayHeadTime = FFrameTime(FFrameNumber(PlayHeadPosition));
	EditorSequencer.Pin()->SetGlobalTime(NewPlayHeadTime);
}

void UStageAPIImpl::ResetSequencer()
{
	API_CHECK_SEQ_VOID

	EditorSequencer.Pin()->Pause();
	EditorSequencer.Pin()->SetGlobalTime(0);
}

void UStageAPIImpl::PlaySequencer()
{
	API_CHECK_SEQ_VOID
	//UE_LOG(LogTemp,Warning,TEXT("ENTERED PLAYSEQUENCER"))

	EditorSequencer.Pin()->OnPlay(false);
}

void UStageAPIImpl::PauseSequencer()
{
	API_CHECK_SEQ_VOID

	EditorSequencer.Pin()->Pause();
}

float UStageAPIImpl::GetSequencerSpeed() const
{
	API_CHECK_SEQ_FLOAT

	return EditorSequencer.Pin()->GetPlaybackSpeed();
}

void UStageAPIImpl::SetSequencerSpeed(float Speed)
{
	API_CHECK_SEQ_VOID

	EditorSequencer.Pin()->SetPlaybackSpeed(Speed);
}

bool UStageAPIImpl::GetSequencerLoop() const
{
	API_CHECK_SEQ_BOOL

	return
		(EditorSequencer.Pin()->GetSequencerSettings()->GetLoopMode() == ESequencerLoopMode::SLM_NoLoop) ? false : true;
}

void UStageAPIImpl::SetSequencerLoop(bool IsLooping)
{
	API_CHECK_SEQ_VOID

	EditorSequencer.Pin()->GetSequencerSettings()->SetLoopMode(IsLooping ? ESequencerLoopMode::SLM_Loop : ESequencerLoopMode::SLM_NoLoop);

}

void UStageAPIImpl::CloseSequencer()
{
	API_CHECK_SEQ_VOID

	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllEditorsForAsset(EditorSequencer.Pin()->GetRootMovieSceneSequence());

}

bool UStageAPIImpl::LoadLevelSequencer(ALevelSequenceActor* LevelSequenceActor)
{
	//TODO: Urgent, find out how UE5 want to load level sequences.
	ULevelSequence* Sequence = Cast<ULevelSequence>(LevelSequenceActor->LevelSequence_DEPRECATED.TryLoad());

	if (!Sequence)
		return false;

	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Sequence);
	
	return true;
}

bool UStageAPIImpl::IsSequencerPlaying() const
{
	API_CHECK_SEQ_BOOL

	return
		(EditorSequencer.Pin()->GetPlaybackStatus() == EMovieScenePlayerStatus::Playing) ?  true :  false;
}

#pragma  endregion




//Send a Take Recorder Start Message via MultiUser
bool UStageAPIImpl::SendMUMessage_TakeRecordStart()
{
	if (TSharedPtr<IConcertSyncClient> ConcertSyncClient = IConcertSyncClientModule::Get().GetClient(TEXT("MultiUser")))
			{
				IConcertClientRef ConcertClient = ConcertSyncClient->GetConcertClient();
				if (TSharedPtr<IConcertClientSession> ConcertClientSession = ConcertClient->GetCurrentSession())
				{
					TSharedRef<IConcertClientSession> MU_Session = ConcertClientSession.ToSharedRef();

					UTakeRecorderPanel* Panel = UTakeRecorderBlueprintLibrary::GetTakeRecorderPanel();
					check(Panel);

					ITakeRecorderModule& TakeRecorderModule = FModuleManager::LoadModuleChecked<ITakeRecorderModule>("TakeRecorder");
					UTakeMetaData* TakeMetaData = Panel->GetTakeMetaData();
					UTakePreset* TakePreset = TakeRecorderModule.GetPendingTake();

					if (TakeMetaData && TakePreset)
					{
						//TakeRecorderModule.OnForceSaveAsPreset().Execute();
						if (TakePreset->GetOutermost()->IsDirty())
						{
							UE_LOG(LogTemp,Warning,TEXT("Cannot start a synchronized take since there are changes to the take preset. Either revert your changes or save the preset to start a synchronized take."))
							return false;
						}

						FConcertTakeInitializedEvent TakeInitializedEvent;
						TakeInitializedEvent.TakeName = Panel->GetName();
						TakeInitializedEvent.TakePresetPath = TakeMetaData->GetPresetOrigin()->GetPathName();
						TakeInitializedEvent.Settings = GetDefault<UTakeRecorderUserSettings>()->Settings;

						//TODO: Cross check what this block is doing.
						FConcertLocalIdentifierTable InLocalIdentifierTable;
						FConcertSyncObjectWriter Writer(&InLocalIdentifierTable, TakeMetaData, TakeInitializedEvent.TakeData, true, false);
						Writer.SerializeObject(TakeMetaData);

						InLocalIdentifierTable.GetState(TakeInitializedEvent.IdentifierState);
						UE_LOG(LogTemp, Display, TEXT("Concert Take Recording START Message Sent"));
						MU_Session->SendCustomEvent(TakeInitializedEvent, MU_Session->GetSessionClientEndpointIds(), EConcertMessageFlags::ReliableOrdered | EConcertMessageFlags::UniqueId);
						return true;
					}
					else
					{
						UE_LOG(LogTemp,Warning,TEXT("Take Recorder Preset or MetaData error"))
						return false;
					}
				}
				else
				{
					UE_LOG(LogTemp,Warning,TEXT("Take Recorder failed to find multi-user session"))
					return false;
				}
			}
			else
			{
				UE_LOG(LogTemp,Warning,TEXT("Take Recorder failed to find multi-user session"))
				return false;
			}
}

//Send a Take Recorder Stop Message via MultiUser
void UStageAPIImpl::SendMUMessage_TakeRecordStop() 
{
	if (TSharedPtr<IConcertSyncClient> ConcertSyncClient = IConcertSyncClientModule::Get().GetClient(TEXT("MultiUser")))
	{
		IConcertClientRef ConcertClient = ConcertSyncClient->GetConcertClient();
		if (TSharedPtr<IConcertClientSession> ConcertClientSession = ConcertClient->GetCurrentSession())
		{
			TSharedRef<IConcertClientSession> MU_Session = ConcertClientSession.ToSharedRef();
			UTakeRecorderPanel* Panel = UTakeRecorderBlueprintLibrary::GetTakeRecorderPanel();
			check(Panel);
					
			FConcertRecordingFinishedEvent RecordingFinishedEvent;
			RecordingFinishedEvent.TakeName = Panel->GetName();
			MU_Session->SendCustomEvent(RecordingFinishedEvent, MU_Session->GetSessionClientEndpointIds(), EConcertMessageFlags::ReliableOrdered | EConcertMessageFlags::UniqueId);
			UE_LOG(LogTemp, Display, TEXT("Concert Take Recording STOP Message Sent"));
		}
	}
}


#pragma endregion //END API Calls











