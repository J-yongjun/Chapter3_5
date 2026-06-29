// SpartaGameState.cpp

#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 10.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	CurrentWaveIndex = 0;

	// 기본 3-웨이브 설정 (에디터 Blueprint에서 덮어쓸 수 있음)
	WaveConfigs.Add({ 10, 20.f }); // Wave 1: 아이템 10개, 20초
	WaveConfigs.Add({ 20, 15.f }); // Wave 2: 아이템 20개, 15초
	WaveConfigs.Add({ 30, 10.f }); // Wave 3: 아이템 30개, 10초
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUD();
	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
	);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
	Score += Amount;
	UE_LOG(LogTemp, Warning, TEXT("Score : %d"), Score);
}

void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	// 웨이브 초기화 후 첫 웨이브 시작
	CurrentWaveIndex = 0;
	StartWave();

	/*TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40;

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameState::OnLevelTimeUp,
		LevelDuration,
		false
	);

	UpdateHUD();

	UE_LOG(LogTemp, Warning, TEXT("Level %d Start!, Spawned %d coin"), 
		CurrentLevelIndex + 1,
		SpawnedCoinCount);*/

}

void ASpartaGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
		CollectedCoinCount,
		SpawnedCoinCount);

	// 현재 레벨에서 스폰된 코인을 전부 주웠다면 즉시 레벨 종료
	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave %d : All coins collected!"), CurrentWaveIndex + 1);
		EndWave();
	}
}

void ASpartaGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);
	GetWorldTimerManager().ClearTimer(WaveTransitionTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	// 레벨 맵 이름이 있다면 해당 맵 불러오기
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		// 맵 이름이 없으면 게임오버
		OnGameOver();
	}
}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::StartWave()
{
	// WaveConfigs가 비어있으면 단일 웨이브로 폴백
	const bool bHasConfigs = WaveConfigs.IsValidIndex(CurrentWaveIndex);

	const int32 ItemToSpawn = bHasConfigs
		? WaveConfigs[CurrentWaveIndex].ItemToSpawn
		: 40;                                           // 기존 기본값
	const float WaveDuration = bHasConfigs
		? WaveConfigs[CurrentWaveIndex].WaveDuration
		: LevelDuration;

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	// SpawnVolume에서 코인 스폰
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() > 0)
	{
		if (ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]))
		{
			for (int32 i = 0; i < ItemToSpawn; i++)
			{
				AActor* Spawned = SpawnVolume->SpawnRandomItem();
				if (Spawned && Spawned->IsA(ACoinItem::StaticClass()))
					SpawnedCoinCount++;
			}
		}
	}

	// 웨이브 타이머 시작
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameState::OnWaveTimeUp,
		WaveDuration,
		false
	);

	UpdateHUD();

	UE_LOG(LogTemp, Warning,
		TEXT("Level : %d, Wave : %d, Start!"),
		CurrentLevelIndex + 1, CurrentWaveIndex + 1);
	GEngine->AddOnScreenDebugMessage(
		-1, 
		2.0f, 
		FColor::Green, 
		FString::Printf(TEXT("Level : %d, Wave : %d, Start!"), CurrentLevelIndex + 1, CurrentWaveIndex + 1));
}

void ASpartaGameState::OnWaveTimeUp()
{
	UE_LOG(LogTemp, Warning, TEXT("Wave %d: Time Up!"), CurrentWaveIndex + 1);
	EndWave();
}

void ASpartaGameState::EndWave()
{
	// 웨이브 타이머 정리
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	CurrentWaveIndex++;

	// 아직 남은 웨이브가 있으면 → 잠깐 딜레이 후 다음 웨이브 시작
	if (WaveConfigs.IsValidIndex(CurrentWaveIndex))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Wave ended. Next wave (%d) starts in 2s..."), CurrentWaveIndex + 1);
		GEngine->AddOnScreenDebugMessage(
			-1, 
			2.0f, 
			FColor::Green, 
			FString::Printf(TEXT("Wave ended. Next wave (%d) starts in 2s..."), CurrentWaveIndex + 1));

		// 2초 딜레이 후 StartWave 호출 (너무 바로 시작되지 않도록)
		GetWorldTimerManager().SetTimer(
			WaveTransitionTimerHandle,
			this,
			&ASpartaGameState::StartWave,
			2.0f,
			false
		);
	}
	else
	{
		// 모든 웨이브 완료 → 레벨 종료
		UE_LOG(LogTemp, Warning, TEXT("All waves cleared! Ending level."));
		EndLevel();
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					if (RemainingTime <= 0)
					{
						TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time : 0.0"))));
					}
					else
					{
						TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time : %.1f"), RemainingTime)));
					}
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level %d"), CurrentLevelIndex + 1)));
				}

				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d"), CurrentWaveIndex + 1)));
				}
			}
		}
	}
}