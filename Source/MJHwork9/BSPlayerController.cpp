#include "BSPlayerController.h"
#include "BSGameModeBase.h"
#include "BSHUDWidget.h"
#include "Blueprint/UserWidget.h"

ABSPlayerController::ABSPlayerController() : HUDWidget(nullptr) {}

void ABSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocalController() && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UBSHUDWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			HUDWidget->SetOwningController(this);
			FInputModeGameAndUI Mode;
			Mode.SetWidgetToFocus(HUDWidget->TakeWidget());
			SetInputMode(Mode);
			bShowMouseCursor = true;
		}
	}
}

void ABSPlayerController::OnSubmitGuess(const FString& Input)
{
	FString Trimmed = Input.TrimStartAndEnd();
	if (!Trimmed.IsEmpty()) Server_SubmitGuess(Trimmed);
}

bool ABSPlayerController::Server_SubmitGuess_Validate(const FString& Input)
{
	return Input.Len() <= 10;
}

void ABSPlayerController::Server_SubmitGuess_Implementation(const FString& Input)
{
	if (ABSGameModeBase* GM = GetWorld()->GetAuthGameMode<ABSGameModeBase>())
		GM->SubmitGuess(this, Input);
}

void ABSPlayerController::Client_ShowMessage_Implementation(const FString& Message)
{
	if (HUDWidget) HUDWidget->AddChatLog(Message);
}

void ABSPlayerController::Client_ShowAnnouncement_Implementation(const FString& Message)
{
	if (HUDWidget) HUDWidget->ShowAnnouncement(Message);
}

void ABSPlayerController::UpdateAttemptUI(int32 Current, int32 Max)
{
	if (HUDWidget) HUDWidget->UpdateAttemptDisplay(Current, Max);
}