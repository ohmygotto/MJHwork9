#include "BSHUDWidget.h"
#include "BSPlayerController.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"

void UBSHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (Button_Submit)
        Button_Submit->OnClicked.AddDynamic(this, &UBSHUDWidget::OnSubmitClicked);
    if (EditableTextBox_Input)
        EditableTextBox_Input->OnTextCommitted.AddDynamic(this, &UBSHUDWidget::OnInputCommitted);
    if (VerticalBox_Announcement)
        VerticalBox_Announcement->SetVisibility(ESlateVisibility::Hidden);
}

void UBSHUDWidget::SetOwningController(ABSPlayerController* InController)
{
    OwningController = InController;
}

void UBSHUDWidget::AddChatLog(const FString& Message)
{
    if (!ScrollBox_ChatLog) return;
    UTextBlock* Line = NewObject<UTextBlock>(this);
    if (Line)
    {
        Line->SetText(FText::FromString(Message));
        ScrollBox_ChatLog->AddChild(Line);
        ScrollBox_ChatLog->ScrollToEnd();
    }
}

void UBSHUDWidget::UpdateAttemptDisplay(int32 Current, int32 Max)
{
    if (TextBlock_AttemptInfo)
        TextBlock_AttemptInfo->SetText(
            FText::FromString(FString::Printf(TEXT("남은 기회: [%d / %d]"), Current, Max)));
}

void UBSHUDWidget::ShowAnnouncement(const FString& Message)
{
    if (VerticalBox_Announcement)
        VerticalBox_Announcement->SetVisibility(ESlateVisibility::Visible);
    if (TextBlock_Announcement)
        TextBlock_Announcement->SetText(FText::FromString(Message));
    if (UWorld* World = GetWorld())
        World->GetTimerManager().SetTimer(
            AnnouncementTimer, this, &UBSHUDWidget::HideAnnouncement, 4.f, false);
}

void UBSHUDWidget::HideAnnouncement()
{
    if (VerticalBox_Announcement)
        VerticalBox_Announcement->SetVisibility(ESlateVisibility::Hidden);
}

void UBSHUDWidget::OnSubmitClicked()
{
    if (!OwningController || !EditableTextBox_Input) return;
    OwningController->OnSubmitGuess(EditableTextBox_Input->GetText().ToString());
    EditableTextBox_Input->SetText(FText::GetEmpty());
}

void UBSHUDWidget::OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter) OnSubmitClicked();
}