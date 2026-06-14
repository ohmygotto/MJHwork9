#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSHUDWidget.generated.h"

class ABSPlayerController;
class UScrollBox;
class UTextBlock;
class UEditableTextBox;
class UButton;
class UVerticalBox;

UCLASS()
class MJHWORK9_API UBSHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetOwningController(ABSPlayerController* InController);
	void AddChatLog(const FString& Message);
	void UpdateAttemptDisplay(int32 Current, int32 Max);
	void ShowAnnouncement(const FString& Message);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))         UScrollBox*       ScrollBox_ChatLog;
	UPROPERTY(meta = (BindWidget))         UTextBlock*       TextBlock_AttemptInfo;
	UPROPERTY(meta = (BindWidget))         UEditableTextBox* EditableTextBox_Input;
	UPROPERTY(meta = (BindWidget))         UButton*          Button_Submit;
	UPROPERTY(meta = (BindWidgetOptional)) UVerticalBox*     VerticalBox_Announcement;
	UPROPERTY(meta = (BindWidgetOptional)) UTextBlock*       TextBlock_Announcement;

private:
	UPROPERTY() ABSPlayerController* OwningController;
	FTimerHandle AnnouncementTimer;

	UFUNCTION() void OnSubmitClicked();
	UFUNCTION() void OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	void HideAnnouncement();
};