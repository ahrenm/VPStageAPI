#pragma once

#include "CoreMinimal.h"
#include "../Public/API/IStageAPIEditor.h"
#include "LevelSequenceActor.h"
#include "StageAPIEditorImpl.generated.h"


class ADisplayClusterRootActor;
class UDisplayClusterICVFXCameraComponent;

UCLASS()
class VPSTAGEAPIEDITOR_API UStageAPIImpl
    : public UObject
    , public IStageAPIEditor
{
    GENERATED_BODY()
        
public:

	~UStageAPIImpl();

	//Validates that the API has found a local NDC actor
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is API Ready"), Category = "VP Stage API")
	virtual bool IsAPIReady() const override;

	//Initalise the API Surface. This call is implicit in all API calls. It should only been needed in specific circumstances where the structure of the NDC actor has changed
    UFUNCTION(BlueprintCallable,
	    meta = (DisplayName = "Init API", ShortTooltip="Implicit in API calls, not needed in most cases"),
	    Category = "VP Stage API")
    virtual bool InitAPISurface() override;


#pragma region "nDisplay API"
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// nDisplay API
	//////////////////////////////////////////////////////////////////////////////////////////////

	////** Gets the Display Cluster Root Actor. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Display Cluster Root Actor"), Category = "VP Stage API|nDisplay")
	virtual ADisplayClusterRootActor* GetDisplayClusterRoot() const override;

	////** Disables all inner frustums. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disable Inner Frustums"), Category = "VP Stage API|nDisplay")
	virtual void DisableInnerFrustum () override;

	////** Enables all inner frustums. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Enable Inner Frustums"), Category = "VP Stage API|nDisplay")
	virtual void EnableInnerFrustum() override;

	////** Gets the enabled/disables state of inner frustums. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Inner Frustum State"), Category = "VP Stage API|nDisplay")
	virtual bool GetInnerFrustumStatus() const override;

	////** Sets the enabled/disables state of inner frustums. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Inner Frustum State"), Category = "VP Stage API|nDisplay")
	virtual void SetInnerFrustumState (bool NewInnerFrustumState) override;

	////** Tries to get the ICVFX Component of the active DisplayClusterRoot */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get ICVFX Component"), Category = "VP Stage API|nDisplay")
	virtual UDisplayClusterICVFXCameraComponent* GetIcvfxCameraComponent() const override;

	////** Tries to get the ICVFX Component of Camera A */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get ICVFX Camera A"), Category = "VP Stage API|nDisplay")
	virtual UDisplayClusterICVFXCameraComponent* GetIcvfxCameraComponentA() const override;

	////** Tries to get the ICVFX Component of Camera B */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get ICVFX Camera B"), Category = "VP Stage API|nDisplay")
	virtual UDisplayClusterICVFXCameraComponent* GetIcvfxCameraComponentB() const override;


	////Returns the list of viewports registered in the cluster
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Viewport Names"), Category = "VP Stage API|nDisplay")
	virtual TArray<FString> GetViewportNames() const override;

	////** Get the Viewport screen percentage multiplier. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Global Screen Percentage"), Category = "VP Stage API|nDisplay")
	virtual float GetGlobalScreenPercentage() const override;

	////** Get the Viewport screen percentage multiplier. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Global Screen Percentage"), Category = "VP Stage API|nDisplay")
	virtual void SetGlobalScreenPercentage (float NewGlobalScreenPercentage) override;

	////** Set the stage position & rotation */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Stage Location"), Category = "VP Stage API|nDisplay")
	virtual void SetStageLocation (FVector StagePosition, FRotator StageRotation) override;

	////** Set the stage position */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Stage Local Position"), Category = "VP Stage API|nDisplay")
	virtual void SetStageLocalPosition (FVector StagePosition) override;

	////** Add a local offset to the stage location */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Stage Local Offset"), Category = "VP Stage API|nDisplay")
	virtual void AddStageLocalOffset (FVector DeltaLocation) override;

	////** Set the stage rotation */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Stage Local Rotation"), Category = "VP Stage API|nDisplay")
	virtual void SetStageLocalRotation (FRotator StageRotation) override;

	////** Get the stage rotation */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Stage Rotation"), Category = "VP Stage API|nDisplay")
	virtual FRotator GetStageLocalRotation() override;

	////** Get the local stage rotation */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Stage World Rotation"), Category = "VP Stage API|nDisplay")
	virtual FRotator GetStageWorldRotation () override;

	////** Set the stage rotation */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Stage World Rotation"), Category = "VP Stage API|nDisplay")
	virtual void SetStageWorldRotation (FRotator StageRotation) override;

	////** Get the stage position */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Stage Rotation"), Category = "VP Stage API|nDisplay")
	virtual FVector GetStagePosition() override ;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Default View Position"), Category = "VP Stage API|nDisplay")
	virtual FVector GetDefaultViewPosition() const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Default View Position"), Category = "VP Stage API|nDisplay")
	virtual void SetDefaultViewPosition(FVector NewPosition) override;


	
#pragma endregion

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Camera API MAIN
	//////////////////////////////////////////////////////////////////////////////////////////////

	////** Gets the Frustum CineCameraActor. */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get Frustum Camera"), Category = "VP Stage API|Frustum Main")
	virtual ACineCameraActor* GetFrustumCamera() const override;

	///Gets the inner frustum field of view multiplier
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum FOV Mult"), Category = "VP Stage API|Frustum Main")
	virtual float GetFrustumFOVMult() const override;

	///Sets the inner frustum field of view multiplier
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum FOV Mult"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumFOVMult(float FOVMult) override;

	///Gets the inner frustum field of view multiplier
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Exposure"), Category = "VP Stage API|Frustum Main")
	virtual float GetFrustumExposure() const override;

	///Gets the frustum aperture
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Aperture"), Category = "VP Stage API|Frustum Main")
	virtual float GetFrustumAperture() const override;

	///Sets the inner frustum field of view multiplier
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Exposure"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumExposure(float FrustumExposure) override;

	///Sets the frustum aperture compensation
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Aperture"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumAperture(float FrustumAperture) override;

	////Sets the Frustum focal distance
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Focal Distance"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumFocalDistance (float FocalDistance) override;

	////Gets the Frustum focal distance
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Focal Distance"), Category = "VP Stage API|Frustum Main")
	virtual float GetFrustumFocalDistance() const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Rotation"), Category = "VP Stage API|Frustum Main")
	virtual FRotator GetFrustumRotation () const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Rotation"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumRotation (FRotator NewRotation) override;
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Rotation Preview"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumRotationPreview (FRotator NewRotation) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Position"), Category = "VP Stage API|Frustum Main")
	virtual FVector GetFrustumPosition () const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Position"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumPosition (FVector NewPosition) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Position Preview"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumPositionPreview (FVector NewPosition) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Render Ratio"), Category = "VP Stage API|Frustum Main")
	virtual float GetFrustumRenderRatio ()  const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Render Ratio"), Category = "VP Stage API|Frustum Main")
	virtual void SetFrustumRenderRatio (float ScreenPercentage) override;

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Camera API CAMERA B
	//////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Camera B Active"), Category = "VP Stage API|Frustum Secondary")
	virtual bool CameraBActive() const  override;

	////** Gets the B Frustum CineCameraActor. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Camera B"), Category = "VP Stage API|Frustum Secondary")
	virtual ACineCameraActor* GetFrustumCameraB() const override;

	///Gets the B inner frustum field of view multiplier
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum FOV Mult B"), Category = "VP Stage API|Frustum Secondary")
	virtual float GetFrustumFOVMultB () const override;

	///Sets the B inner frustum field of view multiplier
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum FOV Mult B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumFOVMultB (float FOVMult) override;

	///Gets the B frustum exposure compensation
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Exposure B"), Category = "VP Stage API|Frustum Secondary")
	virtual float GetFrustumExposureB () const override;

	///Gets the B frustum aperture
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Aperture B"), Category = "VP Stage API|Frustum Secondary")
	virtual float GetFrustumApertureB () const override;

	///Sets the B frustum exposure compensation
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Exposure B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumExposureB (float FrustumExposure) override;

	///Sets the B frustum aperture compensation
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Aperture B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumApertureB (float FrustumAperture) override;

	////Sets the B Frustum focal distance
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Focal Distance B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumFocalDistanceB (float FocalDistance) override;

	////Gets the B Frustum focal distance
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Focal Distance B"), Category = "VP Stage API|Frustum Secondary")
	virtual float GetFrustumFocalDistanceB () const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Rotation B"), Category = "VP Stage API|Frustum Secondary")
	virtual FRotator GetFrustumRotationB () const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Rotation B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumRotationB (FRotator NewRotation) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Rotation Preview B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumRotationPreviewB (FRotator NewRotation) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Position B"), Category = "VP Stage API|Frustum Secondary")
	virtual FVector GetFrustumPositionB () const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Position B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumPositionB (FVector NewPosition) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Position Preview B"), Category = "VP Stage API|Frustum Secondary")
	virtual void SetFrustumPositionPreviewB (FVector NewPosition) override;


	//////////////////////////////////////////////////////////////////////////////////////////////
	// Camera By ICVFX Component
	//
	//These are the underlying calls for Camera(main) and CameraB calls.
	//They can be used by anything that is using an ICVFX component as its root
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Camera"), Category = "VP Stage API|Frustum by ICVFX")
	virtual ACineCameraActor* GetFrustumCamera_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent)const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum FOV Mult"), Category = "VP Stage API|Frustum by ICVFX")
	virtual float GetFrustumFOVMult_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum FOV Mult"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumFOVMult_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, float FOVMult) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Exposure"), Category = "VP Stage API|Frustum by ICVFX")
	virtual float GetFrustumExposure_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Aperture"), Category = "VP Stage API|Frustum by ICVFX")
	virtual float GetFrustumAperture_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Exposure"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumExposure_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, float FrustumExposure) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Aperture"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumAperture_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, float FrustumAperture) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Focal Distance"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumFocalDistance_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent,float FocalDistance) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Focal Distance"), Category = "VP Stage API|Frustum by ICVFX")
	virtual float GetFrustumFocalDistance_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Rotation"), Category = "VP Stage API|Frustum by ICVFX")
	virtual FRotator GetFrustumRotation_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Rotation"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumRotation_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent,FRotator NewRotation) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Rotation Preview"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumRotationPreview_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent,FRotator NewRotation) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Frustum Position"), Category = "VP Stage API|Frustum by ICVFX")
	virtual FVector GetFrustumPosition_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent) const override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Position"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumPosition_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, FVector NewPosition) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Frustum Position Preview"), Category = "VP Stage API|Frustum by ICVFX")
	virtual void SetFrustumPositionPreview_ByComponent(UDisplayClusterICVFXCameraComponent* IcvfxComponent, FVector NewPosition) override;


#pragma region "Image & Color API"

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Image & Color API
	//////////////////////////////////////////////////////////////////////////////////////////////

	////Get the stage exposure compensation
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Stage Exposure Compensation"), Category = "VP Stage API|Image & Color")
	virtual float GetStageExposure() const override;

	////** Set the stage exposure compensation level */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Stage Exposure Compensation"), Category = "VP Stage API|Image & Color")
	virtual void SetStageExposure(float ExposureCompensation ) override;

	////** Set the exposure level of the stage camera. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disable Stage Exposure Compensation"), Category = "VP Stage API|Image & Color")
	virtual void DisableStageExposure() override;

	////** Sets the frustum greenscreen enabled state. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Chromakey Enabled"), Category = "VP Stage API|Image & Color")
	virtual void SetChromakeyStatus(bool ChromakeyEnabled) override;

	////Disable the Chromakey overlay
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disable Chromakey"), Category = "VP Stage API|Image & Color")
	virtual void DisableChromakey() override;

	//Enable the Chromakey overlay
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Enable Chromakey"), Category = "VP Stage API|Image & Color")
	virtual void EnableChromakey() override;

	//Get the status of the Chormakey overlay
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Chromakey Status"), Category = "VP Stage API|Image & Color")
	virtual bool GetChromakeyStatus () const override;

	//CLUSTER GRADE FUNCTIONS

	//Get the Cluster grade saturation
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Saturation"), Category="VP Stage API|Image & Color")
	virtual FVector4 GetClusterPP_GlobalSaturation() const override;

	//Set the Cluster grade saturation
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Saturation"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_GlobalSaturation(FVector4 NewSaturation) override;

	//Get the Cluster grade Contrast
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Contrast"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_GlobalContrast() const override;

	//Set the Cluster grade Contrast
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Contrast"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_GlobalContrast(FVector4 NewContrast) override;

	//Get the Cluster grade Gamma
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Gamma"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_GlobalGamma() const override;

	//Set the Cluster grade Gamma
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Gamma"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_GlobalGamma(FVector4 NewGamma) override;

	//Get the Cluster grade Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_GlobalGain() const override;

	//Set the Cluster grade Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Gain"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_GlobalGain(FVector4 NewGain) override;

	//Get the Cluster grade Offset
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Contrast"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_GlobalOffset() const override;

	//Set the Cluster grade Offset
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Offset"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_GlobalOffset(FVector4 NewOffset) override;

	//Get the Cluster grade Shadows Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Shadows Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_ShadowsGain() const override;

	//Set the Cluster grade Shadows Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Shadows Gain"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_ShadowsGain(FVector4 NewShadowsGain) override;

	//Get the Cluster grade Mids Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Mids Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_MidsGain() const override;

	//Set the Cluster grade Mids Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Mids Gain"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_MidsGain(FVector4 NewMidsGain) override;

	//Get the Cluster grade Highlights Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Cluster Highlights Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetClusterPP_HighlightsGain() const override;

	//Set the Cluster grade Highlights Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Cluster Highlights Gain"), Category="VP Stage API|Image & Color")
	virtual void SetClusterPP_HighlightsGain(FVector4 NewHighlightsGain) override;

	//FRUSTUM GRADE FUNCTIONS

	//Get the Frustum grade saturation
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Saturation"), Category="VP Stage API|Image & Color")
	virtual FVector4 GetFrustumPP_GlobalSaturation() const override;

	//Set the Frustum grade saturation
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Saturation"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_GlobalSaturation(FVector4 NewSaturation) override;

	//Get the Frustum grade Contrast
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Contrast"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_GlobalContrast() const override;

	//Set the Frustum grade Contrast
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Contrast"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_GlobalContrast(FVector4 NewContrast) override;

	//Get the Frustum grade Gamma
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Gamma"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_GlobalGamma() const override;

	//Set the Frustum grade Gamma
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Gamma"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_GlobalGamma(FVector4 NewGamma) override;

	//Get the Frustum grade Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_GlobalGain() const override;

	//Set the Frustum grade Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Gain"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_GlobalGain(FVector4 NewGain) override;

	//Get the Frustum grade Offset
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Contrast"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_GlobalOffset() const override;

	//Set the Frustum grade Offset
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Offset"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_GlobalOffset(FVector4 NewOffset) override;

	//Get the Frustum grade Shadows Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Shadows Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_ShadowsGain() const override;

	//Set the Frustum grade Shadows Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Shadows Gain"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_ShadowsGain(FVector4 NewShadowsGain) override;

	//Get the Frustum grade Mids Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Mids Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_MidsGain() const override;

	//Set the Frustum grade Mids Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Mids Gain"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_MidsGain(FVector4 NewMidsGain) override;

	//Get the Frustum grade Highlights Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Frustum Highlights Gain"), Category="VP Stage API|Image & Color")
	virtual  FVector4 GetFrustumPP_HighlightsGain() const override;

	//Set the Frustum grade Highlights Gain
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Frustum Highlights Gain"), Category="VP Stage API|Image & Color")
	virtual void SetFrustumPP_HighlightsGain(FVector4 NewHighlightsGain) override;


#pragma  endregion 

	
#pragma region "Sequencer Controls"

	//SEQUENCER CONTROLS
	
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Sequencer Position Seconds"), Category="VP Stage API|Sequencer")
	virtual float GetSequencerTime() const override;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Sequencer Position Seconds"), Category="VP Stage API|Sequencer")
	virtual void SetSequencerTime (float PlayHeadInSeconds) override;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Reset Sequencer"), Category="VP Stage API|Sequencer")
	virtual void ResetSequencer()override;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Play Sequencer"), Category="VP Stage API|Sequencer")
	virtual void PlaySequencer() override;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Pause Sequencer"), Category="VP Stage API|Sequencer")
	virtual void PauseSequencer() override;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Sequencer Speed"), Category="VP Stage API|Sequencer")
	virtual float GetSequencerSpeed () const override;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Sequencer Speed"), Category="VP Stage API|Sequencer")
	virtual void SetSequencerSpeed(float Speed) override;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Sequencer Loop"), Category="VP Stage API|Sequencer")
	virtual bool GetSequencerLoop () const override;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Sequencer Loop"), Category="VP Stage API|Sequencer")
	virtual void SetSequencerLoop(bool IsLooping) override;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Close Sequencer"), Category="VP Stage API|Sequencer")
	virtual void CloseSequencer() override;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Load Level Sequence"), Category="VP Stage API|Sequencer")
	virtual bool LoadLevelSequencer(ALevelSequenceActor* LevelSequenceActor) override;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Is Sequencer Playing"), Category="VP Stage API|Sequencer")
	virtual bool IsSequencerPlaying () const override;

#pragma  endregion

	//Send a Take Recorder Start Message via MultiUser
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Take Record Start via MU"), Category="VP Stage API|Misc")
	virtual bool SendMUMessage_TakeRecordStart() override;

	//Send a Take Recorder Stop Message via MultiUser
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Take Record Stop via MU"), Category="VP Stage API|Misc")
	virtual void SendMUMessage_TakeRecordStop() override;

private:
	
};