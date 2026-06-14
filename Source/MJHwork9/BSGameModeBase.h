#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BSGameModeBase.generated.h"

UCLASS()
class MJHWORK9_API ABSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ABSGameModeBase();
	virtual void BeginPlay() override;

	void SubmitGuess(APlayerController* SubmittingPC, const FString& Input);
	void ResetGame();

private:
	TArray<int32> SecretNumbers;
	bool bGameOver;

	void GenerateSecretNumbers();
	FString ValidateInput(const FString& Input) const;
	FString CheckAnswer(const TArray<int32>& Guess) const;
	void CheckGameOver();
	void BroadcastResult(APlayerController* TargetPC, const FString& Message);
	void BroadcastAnnouncement(const FString& Message);
};