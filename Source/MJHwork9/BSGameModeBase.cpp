#include "BSGameModeBase.h"
#include "BSPlayerController.h"
#include "BSPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"   // ← 이 줄 추가

ABSGameModeBase::ABSGameModeBase()
{
    PlayerControllerClass = ABSPlayerController::StaticClass();
    PlayerStateClass = ABSPlayerState::StaticClass();
    bGameOver = false;
}

void ABSGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    GenerateSecretNumbers();
}

void ABSGameModeBase::GenerateSecretNumbers()
{
    SecretNumbers.Empty();
    while (SecretNumbers.Num() < 3)
    {
        int32 Candidate = FMath::RandRange(1, 9);
        if (!SecretNumbers.Contains(Candidate))
            SecretNumbers.Add(Candidate);
    }
    UE_LOG(LogTemp, Warning, TEXT("[서버] 정답: %d%d%d"),
        SecretNumbers[0], SecretNumbers[1], SecretNumbers[2]);
}

FString ABSGameModeBase::ValidateInput(const FString& Input) const
{
    if (Input.Len() != 3)
        return TEXT("⚠️ 3자리 숫자를 입력해주세요.");

    TArray<int32> Digits;
    for (int32 i = 0; i < 3; ++i)
    {
        TCHAR Ch = Input[i];
        if (!FChar::IsDigit(Ch))
            return TEXT("⚠️ 숫자만 입력해주세요.");

        int32 Digit = Ch - '0';
        if (Digit == 0)
            return TEXT("⚠️ 1~9 사이의 숫자를 입력해주세요.");
        if (Digits.Contains(Digit))
            return TEXT("⚠️ 중복되지 않은 숫자를 입력해주세요.");
        Digits.Add(Digit);
    }
    return TEXT("");
}

FString ABSGameModeBase::CheckAnswer(const TArray<int32>& Guess) const
{
    int32 Strikes = 0, Balls = 0;
    for (int32 i = 0; i < 3; ++i)
    {
        if (Guess[i] == SecretNumbers[i])          ++Strikes;
        else if (SecretNumbers.Contains(Guess[i])) ++Balls;
    }
    if (Strikes == 0 && Balls == 0) return TEXT("OUT");
    return FString::Printf(TEXT("%dS%dB"), Strikes, Balls);
}

void ABSGameModeBase::SubmitGuess(APlayerController* SubmittingPC, const FString& Input)
{
    if (!SubmittingPC) return;
    ABSPlayerState* PS = SubmittingPC->GetPlayerState<ABSPlayerState>();
    if (!PS) return;

    if (bGameOver)
    {
        BroadcastResult(SubmittingPC, TEXT("🔒 게임 종료. 잠시 후 재시작됩니다."));
        return;
    }
    if (PS->GetAttemptCount() >= PS->GetMaxAttempts())
    {
        BroadcastResult(SubmittingPC, TEXT("🔒 기회를 모두 소진했습니다."));
        return;
    }

    FString Error = ValidateInput(Input);
    if (!Error.IsEmpty())
    {
        if (ABSPlayerController* BSPC = Cast<ABSPlayerController>(SubmittingPC))
            BSPC->Client_ShowMessage(Error);
        return;
    }

    PS->IncrementAttempt();

    TArray<int32> Guess;
    for (TCHAR Ch : Input) Guess.Add(Ch - '0');

    FString Result = CheckAnswer(Guess);
    FString Msg = FString::Printf(TEXT("🎯 %s [%d/%d] 입력: %s → %s"),
        *PS->GetPlayerName(), PS->GetAttemptCount(), PS->GetMaxAttempts(),
        *Input, *Result);

    BroadcastResult(nullptr, Msg);

    if (Result == TEXT("3S0B"))
    {
        bGameOver = true;
        BroadcastAnnouncement(FString::Printf(TEXT("🏆 %s 승리! 정답: %d%d%d"),
            *PS->GetPlayerName(),
            SecretNumbers[0], SecretNumbers[1], SecretNumbers[2]));
        FTimerHandle T;
        GetWorldTimerManager().SetTimer(T, this, &ABSGameModeBase::ResetGame, 5.f, false);
        return;
    }
    CheckGameOver();
}

void ABSGameModeBase::CheckGameOver()
{
    if (bGameOver) return;

    // ↓ 수정: GameState-> 대신 GetGameState<AGameStateBase>() 사용
    AGameStateBase* GS = GetGameState<AGameStateBase>();
    if (!GS) return;

    for (APlayerState* PS : GS->PlayerArray)
    {
        ABSPlayerState* BSPS = Cast<ABSPlayerState>(PS);
        if (BSPS && BSPS->GetAttemptCount() < BSPS->GetMaxAttempts()) return;
    }
    bGameOver = true;
    BroadcastAnnouncement(FString::Printf(TEXT("🤝 무승부! 정답: %d%d%d"),
        SecretNumbers[0], SecretNumbers[1], SecretNumbers[2]));
    FTimerHandle T;
    GetWorldTimerManager().SetTimer(T, this, &ABSGameModeBase::ResetGame, 5.f, false);
}

void ABSGameModeBase::ResetGame()
{
    bGameOver = false;

    // ↓ 수정: 동일하게 GetGameState<AGameStateBase>() 사용
    AGameStateBase* GS = GetGameState<AGameStateBase>();
    if (GS)
    {
        for (APlayerState* PS : GS->PlayerArray)
            if (ABSPlayerState* BSPS = Cast<ABSPlayerState>(PS))
                BSPS->ResetAttempts();
    }
    GenerateSecretNumbers();
    BroadcastAnnouncement(TEXT("🔄 새 게임 시작! 3자리 숫자를 맞혀보세요!"));
}

void ABSGameModeBase::BroadcastResult(APlayerController* TargetPC, const FString& Message)
{
    auto SendTo = [&](APlayerController* PC) {
        if (ABSPlayerController* BSPC = Cast<ABSPlayerController>(PC))
            BSPC->Client_ShowMessage(Message);
    };
    if (TargetPC) { SendTo(TargetPC); return; }
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        SendTo(It->Get());
}

void ABSGameModeBase::BroadcastAnnouncement(const FString& Message)
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        if (ABSPlayerController* BSPC = Cast<ABSPlayerController>(It->Get()))
            BSPC->Client_ShowAnnouncement(Message);
}