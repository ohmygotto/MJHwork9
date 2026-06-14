#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BSPlayerController.generated.h"

class UBSHUDWidget;

UCLASS()
class MJHWORK9_API ABSPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ABSPlayerController();
	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable)
	void Client_ShowMessage(const FString& Message);

	UFUNCTION(Client, Reliable)
	void Client_ShowAnnouncement(const FString& Message);

	void UpdateAttemptUI(int32 Current, int32 Max);
	void OnSubmitGuess(const FString& Input);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBSHUDWidget> HUDWidgetClass;

private:
	UPROPERTY()
	UBSHUDWidget* HUDWidget;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SubmitGuess(const FString& Input);
	bool Server_SubmitGuess_Validate(const FString& Input);
	void Server_SubmitGuess_Implementation(const FString& Input);
};