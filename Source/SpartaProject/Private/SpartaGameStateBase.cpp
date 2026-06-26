// SpartaGameStateBase.cpp


#include "SpartaGameStateBase.h"

ASpartaGameStateBase::ASpartaGameStateBase()
{
	Score = 0;
}

int32 ASpartaGameStateBase::GetScore() const
{
	return Score;
}

void ASpartaGameStateBase::AddScore(int32 Amount)
{
	Score += Amount;
	UE_LOG(LogTemp, Warning, TEXT("Score : %d"), Score);
}