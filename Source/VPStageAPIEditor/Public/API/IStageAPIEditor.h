#pragma once

#include "CoreMinimal.h"
#include "CineCameraActor.h"
#include "LevelSequenceActor.h"

#include "IStageAPIEditor.generated.h"

class ADisplayClusterRootActor;
class UDisplayClusterICVFXCameraComponent;

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class VPSTAGEAPIEDITOR_API UStageAPIEditor : public UInterface
{
	GENERATED_BODY()
};
 
class VPSTAGEAPIEDITOR_API IStageAPIEditor
{
	GENERATED_BODY()
	
public:

	//Validates that the API has found a local NDC actor
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is API Ready"), Category = "VP Stage API")
	virtual bool IsAPIReady() const = 0;

	//Initalise the API Surface. This call is implicit in all API calls. It should only been needed in specific circumstances where the structure of the NDC actor has changed
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Init API",ShortTooltip="Implicit in API calls, not needed in most cases"), Category = "VP Stage API")
	virtual bool InitAPISurface() = 0;


#pragma region "nDisplay API"
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// nDisplay API
	//////////////////////////////////////////////////////////////////////////////////////////////

	////** Gets the Display Cluster Root Actor. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Display Cluster Root Actor"), Category = "VP Stage API|nDisplay")
	virtual ADisplayClusterRootActor* GetDisplayClusterRoot() const = 0;

	////** Disables all inner frustums. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disable Inner Frustums"), Category = "VP Stage API|nDisplay")
	virtual void DisableInnerFrustum () = 0;

	////** Enables all inner frustums. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Enable Inner Frustums"), Category = "VP Stage API|nDisplay")
	virtual void EnableInnerFrustum () =0;

	////** Gets the enabled/disables state of inner frustums. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Inner Frustum State"), Category = "VP Stage API|nDisplay")
	virtual bool GetInnerFrustumStatus() const =0;

	////** Sets the enabled/disables state of inner frustums. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Inner Frustum State"), Category = "VP Stage API|nDisplay")
	virtual void SetInnerFrustumState (bool NewInnerFrustumState) = 0;

	////** Tries to get the ICVFX Component of the active DisplayClusterRoot */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get ICVFX Component"), Category = "VP Stage API|nDisplay")
	virtual UDisplayClusterICVFXCameraComponent* GetIcvfxCameraComponent() const =0;

	////** Tries to get the ICVFX Component of Camera A */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get ICVFX Camera A"), Category = "VP Stage API|nDisplay")
	virtual UDisplayClusterICVFXCameraComponent* GetIcvfxCameraComponentA() const =0;

	////** Tries to get the ICVFX Component of Camera B */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get ICVFX Camera B"), Category = "VP Stage API|nDisplay")
	virtual UDisplayClusterICVFXCameraComponent* GetIcvfxCameraComponentB() const =0;

	////Returns the list of viewports registered in the cluster
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Viewport Names"), Category = "VP Stage API|nDisplay")
	virtual TArray<FString> GetViewportNames() const =0;

	////** Get the Viewport screen percentage multiplier. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Global Screen Percentage"), Category = "VP Stage API|nDisplay")
	virtual float GetGlobalScreenPercentage() const =0;

	////** Get the Viewport screen percentage multiplier. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Global Screen Percentage"), Category = "VP Stage API|nDisplay")
	virtual void SetGlobalScreenPercentage (float NewGlobalScreenPercentage) = 0;

	////** Set the stage position & rotation */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Stage Location"), Category = "VP Stage API|nDisplay")
	virtual void SetStageLocation (FVector StagePosition, FRotator StageRotation) = 0;

	////** Set the stage position */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Stage Local Position"), Category = "VP Stage API|nDisplay")
	virtual void SetStageLocalPosition (FVector StagePosition) = 0;
	
	////** Add a local offset to the stage location */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Stage Local Offset"), Category = "VP Stage API|nDisplay")
	virtual void AddStageLocalOffset (FVector DeltaLocation) = 0;

	////** Set the stage rotation */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Stage Local Rotation"), Category = "VP Stage API|nDisplay")
	virtual void SetStageLocalRotation (FRotator StageRotation) = 0;

	////** Get the local stage rotation */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Stage Local Rotation"), Category = "VP Stage API|nDisplay")
	virtual FRotator GetStageLocalRotation () = 0;

	////** Get the local stage rotation */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Stage World Rotation"), Category = "VP Stage API|nDisplay")
	virtual FRotator GetStageWorldRotation () = 0;

	////** Set the stage rotation */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Stage World Rotation"), Category = "VP Stage API|nDisplay")
	virtual void SetStageWorldRotation (FRotator StageRotation) = 0;

	////** Get the stage position */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Stage Local Position"), Category = "VP Stage API|nDisplay")
	virtual FVector GetStagePosition () = 0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Default View Position"), Category = "VP Stage API|nDisplay")
	virtual FVector GetDefaultViewPosition() const = 0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Default View Position"), Category = "VP Stage API|nDisplay")
	virtual void SetDefaultViewPosition(FVector NewPosition) = 0;

	
#pragma endregion 	

#pragma region "Frustum/ICVFX API"
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Camera API MAIN
	//////////////////////////////////////////////////////////////////////////////////////////////

	////** Gets the Frustum CineCameraActor. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Camera"), Category = "VP Stage API|Frustum Main")
	virtual ACineCameraActor* GetFrustumCamera() const =0;

	///Gets the inner frustum field of view multiplier
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum FOV Mult"), Category = "VP Stage API|Frustum Main")
	virtual float GetFrustumFOVMult() const =0;

	///Sets the inner frustum field of view multiplier
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum FOV Mult"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumFOVMult(float FOVMult) =0;

	///Gets the frustum exposure compensation
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Exposure"), Category = "VP Stage API|Frustum Main")
	virtual float GetFrustumExposure() const =0;

	///Gets the frustum aperture
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Aperture"), Category = "VP Stage API|Frustum Main")
	virtual float GetFrustumAperture() const =0;

	///Sets the frustum exposure compensation
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Exposure"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumExposure(float FrustumExposure) =0;

	///Sets the frustum aperture compensation
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Aperture"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumAperture(float FrustumAperture) =0;

	////Sets the Frustum focal distance
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Focal Distance"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumFocalDistance (float FocalDistance) = 0;

	////Gets the Frustum focal distance
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Focal Distance"), Category = "VP Stage API|Frustum Main")
	virtual float GetFrustumFocalDistance() const = 0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Rotation"), Category = "VP Stage API|Frustum Main")
	virtual FRotator GetFrustumRotation () const =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Rotation"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumRotation (FRotator NewRotation) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Rotation Preview"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumRotationPreview (FRotator NewRotation) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Position"), Category = "VP Stage API|Frustum Main")
	virtual FVector GetFrustumPosition ()  const =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Position"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumPosition (FVector NewPosition) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Position Preview"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumPositionPreview (FVector NewPosition) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Render Ratio"), Category = "VP Stage API|Frustum Main")
	virtual float GetFrustumRenderRatio ()  const =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Render Ratio"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumRenderRatio (float RenderRation) =0;


	//////////////////////////////////////////////////////////////////////////////////////////////
	// Camera API CAMERA B
	//////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Camera B Active"), Category = "VP Stage API|Frustum Secondary")
	virtual bool CameraBActive() const =0;
	
	////** Gets the B Frustum CineCameraActor. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Camera B"), Category = "VP Stage API|Frustum Secondary")
	virtual ACineCameraActor* GetFrustumCameraB() const =0;

	///Gets the B inner frustum field of view multiplier
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum FOV Mult B"), Category = "VP Stage API|Frustum Secondary")
	virtual float GetFrustumFOVMultB () const =0;

	///Sets the B inner frustum field of view multiplier
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum FOV Mult B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumFOVMultB (float FOVMult) =0;

	///Gets the B frustum exposure compensation
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Exposure B"), Category = "VP Stage API|Frustum Secondary")
	virtual float GetFrustumExposureB () const =0;

	///Gets the B frustum aperture
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Aperture B"), Category = "VP Stage API|Frustum Secondary")
	virtual float GetFrustumApertureB () const =0;

	///Sets the B frustum exposure compensation
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Exposure B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumExposureB (float FrustumExposure) =0;

	///Sets the B frustum aperture compensation
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Aperture B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumApertureB (float FrustumAperture) =0;

	////Sets the B Frustum focal distance
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Focal Distance B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumFocalDistanceB (float FocalDistance) = 0;

	////Gets the B Frustum focal distance
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Focal Distance B"), Category = "VP Stage API|Frustum Secondary")
	virtual float GetFrustumFocalDistanceB () const = 0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Rotation B"), Category = "VP Stage API|Frustum Secondary")
	virtual FRotator GetFrustumRotationB () const =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Rotation B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumRotationB (FRotator NewRotation) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Rotation Preview B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumRotationPreviewB (FRotator NewRotation) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Position B"), Category = "VP Stage API|Frustum Secondary")
	virtual FVector GetFrustumPositionB () const =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Position B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumPositionB (FVector NewPosition) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Position Preview B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumPositionPreviewB (FVector NewPosition) =0;


	//////////////////////////////////////////////////////////////////////////////////////////////
	// Camera By ICVFX Component
	//
	//These are the underlying calls for Camera(main) and CameraB calls.
	//They can be used by anything that is using an ICVFX component as its root
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Camera"), Category = "VP Stage API|Frustum by ICVFX")
	virtual ACineCameraActor* GetFrustumCamera_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent)const =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum FOV Mult"), Category = "VP Stage API|Frustum by ICVFX")
	virtual float GetFrustumFOVMult_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum FOV Mult"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumFOVMult_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, float FOVMult) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Exposure"), Category = "VP Stage API|Frustum by ICVFX")
	virtual float GetFrustumExposure_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Aperture"), Category = "VP Stage API|Frustum by ICVFX")
	virtual float GetFrustumAperture_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Exposure"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumExposure_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, float FrustumExposure) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Aperture"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumAperture_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, float FrustumAperture) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Focal Distance"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumFocalDistance_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent,float FocalDistance) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Focal Distance"), Category = "VP Stage API|Frustum by ICVFX")
	virtual float GetFrustumFocalDistance_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Rotation"), Category = "VP Stage API|Frustum by ICVFX")
	virtual FRotator GetFrustumRotation_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Rotation"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumRotation_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent,FRotator NewRotation) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Rotation Preview"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumRotationPreview_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent,FRotator NewRotation) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Position"), Category = "VP Stage API|Frustum by ICVFX")
	virtual FVector GetFrustumPosition_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Position"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumPosition_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, FVector NewPosition) =0;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Position Preview"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumPositionPreview_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, FVector NewPosition) =0;

#pragma endregion 	

#pragma region "Image & Color API"

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Image & Color API
	//////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Stage Exposure Compensation"), Category = "VP Stage API|Image & Color")
	virtual float GetStageExposure() const =0;

	////** Set the exposure level of the stage camera. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Stage Exposure Compensation"), Category = "VP Stage API|Image & Color")
	virtual void SetStageExposure(float Exposure ) =0;

	////** Set the exposure level of the stage camera. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disable Stage Exposure Compensation"), Category = "VP Stage API|Image & Color")
	virtual void DisableStageExposure() =0;

	////** Sets the frustum greenscreen enabled state. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Chromakey Enabled"), Category = "VP Stage API|Image & Color")
	virtual void SetChromakeyStatus(bool ChromakeyEnabled) =0;

	////Disable the Chromakey overlay
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disable Chromakey"), Category = "VP Stage API|Image & Color")
	virtual void DisableChromakey() =0;

	//Enable the Chromakey overlay
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Enable Chromakey"), Category = "VP Stage API|Image & Color")
	virtual void EnableChromakey() = 0;

	//Get the status of the Chormakey overlay
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Chromakey Status"), Category = "VP Stage API|Image & Color")
	virtual bool GetChromakeyStatus () const =0;

	//CLUSTER GRADE FUNCTIONS

	//Get the Cluster grade saturation
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Saturation"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_GlobalSaturation() const = 0;

	//Set the Cluster grade saturation
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Saturation"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_GlobalSaturation(FVector4 NewSaturation) = 0;

	//Get the Cluster grade Contrast
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Contrast"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_GlobalContrast() const = 0;

	//Set the Cluster grade Contrast
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Contrast"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_GlobalContrast(FVector4 NewContrast) = 0;

	//Get the Cluster grade Gamma
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Gamma"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_GlobalGamma() const = 0;

	//Set the Cluster grade Gamma
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Gamma"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_GlobalGamma(FVector4 NewGamma) = 0;

	//Get the Cluster grade Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_GlobalGain() const = 0;

	//Set the Cluster grade Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Gain"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_GlobalGain(FVector4 NewGain) = 0;

	//Get the Cluster grade Offset
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Contrast"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_GlobalOffset() const = 0;

	//Set the Cluster grade Offset
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Offset"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_GlobalOffset(FVector4 NewOffset) = 0;

	//Get the Cluster grade Shadows Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Shadows Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_ShadowsGain() const = 0;

	//Set the Cluster grade Shadows Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Shadows Gain"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_ShadowsGain(FVector4 NewShadowsGain) = 0;

	//Get the Cluster grade Mids Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Mids Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_MidsGain() const = 0;

	//Set the Cluster grade Mids Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Mids Gain"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_MidsGain(FVector4 NewMidsGain) = 0;

	//Get the Cluster grade Highlights Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Highlights Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_HighlightsGain() const = 0;

	//Set the Cluster grade Highlights Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Highlights Gain"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_HighlightsGain(FVector4 NewHighlightsGain) = 0;

	//FRUSTUM GRADE FUNCTIONS

	//Get the Frustum grade saturation
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Saturation"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_GlobalSaturation() const = 0;

	//Set the Frustum grade saturation
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Saturation"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_GlobalSaturation(FVector4 NewSaturation) = 0;

	//Get the Frustum grade Contrast
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Contrast"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_GlobalContrast() const = 0;

	//Set the Frustum grade Contrast
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Contrast"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_GlobalContrast(FVector4 NewContrast) = 0;

	//Get the Frustum grade Gamma
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Gamma"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_GlobalGamma() const = 0;

	//Set the Frustum grade Gamma
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Gamma"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_GlobalGamma(FVector4 NewGamma) = 0;

	//Get the Frustum grade Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_GlobalGain() const = 0;

	//Set the Frustum grade Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Gain"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_GlobalGain(FVector4 NewGain) = 0;

	//Get the Frustum grade Offset
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Contrast"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_GlobalOffset() const = 0;

	//Set the Frustum grade Offset
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Offset"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_GlobalOffset(FVector4 NewOffset) = 0;

	//Get the Frustum grade Shadows Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Shadows Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_ShadowsGain() const = 0;

	//Set the Frustum grade Shadows Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Shadows Gain"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_ShadowsGain(FVector4 NewShadowsGain) = 0;

	//Get the Frustum grade Mids Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Mids Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_MidsGain() const = 0;

	//Set the Frustum grade Mids Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Mids Gain"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_MidsGain(FVector4 NewMidsGain) = 0;

	//Get the Frustum grade Highlights Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Highlights Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_HighlightsGain() const = 0;

	//Set the Frustum grade Highlights Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Highlights Gain"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_HighlightsGain(FVector4 NewHighlightsGain) = 0;


#pragma  endregion

#pragma region "Sequencer Controls"
	
	//SEQUENCER CONTROLS
	
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Sequencer Position Seconds"), Category="VP Stage API|Sequencer")
	virtual float GetSequencerTime() const = 0;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Sequencer Position Seconds"), Category="VP Stage API|Sequencer")
	virtual void SetSequencerTime (float PlayHeadInSeconds) =0;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Reset Sequencer"), Category="VP Stage API|Sequencer")
	virtual void ResetSequencer() =0;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Play Sequencer"), Category="VP Stage API|Sequencer")
	virtual void PlaySequencer() =0;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Pause Sequencer"), Category="VP Stage API|Sequencer")
	virtual void PauseSequencer() = 0;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Sequencer Speed"), Category="VP Stage API|Sequencer")
	virtual float GetSequencerSpeed () const = 0;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Sequencer Speed"), Category="VP Stage API|Sequencer")
	virtual void SetSequencerSpeed(float Speed) = 0;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Sequencer Loop"), Category="VP Stage API|Sequencer")
	virtual bool GetSequencerLoop () const = 0;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Sequencer Loop"), Category="VP Stage API|Sequencer")
	virtual void SetSequencerLoop(bool IsLooping) = 0;
	
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Close Sequencer"), Category="VP Stage API|Sequencer")
	virtual void CloseSequencer() = 0;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Load Level Sequence"), Category="VP Stage API|Sequencer")
	virtual bool LoadLevelSequencer(ALevelSequenceActor* LevelSequenceActor) = 0;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Is Sequencer Playing"), Category="VP Stage API|Sequencer")
	virtual bool IsSequencerPlaying () const = 0;

#pragma  endregion

	//Send a Take Recorder Start Message via MultiUser
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Take Record Start via MU"), Category="VP Stage API|Misc")
	virtual bool SendMUMessage_TakeRecordStart() =0;

	//Send a Take Recorder Stop Message via MultiUser
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Take Record Stop via MU"), Category="VP Stage API|Misc")
	virtual void  SendMUMessage_TakeRecordStop() =0;
	
};
