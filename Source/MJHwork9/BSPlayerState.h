#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BSPlayerState.generated.h"

UCLASS()
class MJHWORK9_API ABSPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ABSPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	int32 GetAttemptCount() const { return AttemptCount; }
	int32 GetMaxAttempts()  const { return MaxAttempts; }
	bool  HasUsedAllAttempts() const { return AttemptCount >= MaxAttempts; }

	void IncrementAttempt();
	void ResetAttempts();

private:
	UPROPERTY(ReplicatedUsing = OnRep_AttemptCount)
	int32 AttemptCount;

	UPROPERTY(Replicated)
	int32 MaxAttempts;

	UFUNCTION()
	void OnRep_AttemptCount();
};
