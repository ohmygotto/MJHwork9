#include "BSPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "BSPlayerController.h"

ABSPlayerState::ABSPlayerState()
{
	AttemptCount = 0;
	MaxAttempts  = 3;
}

void ABSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSPlayerState, AttemptCount);
	DOREPLIFETIME(ABSPlayerState, MaxAttempts);
}

void ABSPlayerState::IncrementAttempt()
{
	if (GetLocalRole() == ROLE_Authority)
		AttemptCount = FMath::Min(AttemptCount + 1, MaxAttempts);
}

void ABSPlayerState::ResetAttempts()
{
	if (GetLocalRole() == ROLE_Authority)
		AttemptCount = 0;
}

void ABSPlayerState::OnRep_AttemptCount()
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
		if (ABSPlayerController* BSPC = Cast<ABSPlayerController>(PC))
			BSPC->UpdateAttemptUI(AttemptCount, MaxAttempts);
}