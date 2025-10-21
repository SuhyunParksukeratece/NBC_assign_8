// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "SpartaCharacter.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30.0f;
	StartDuration = 5.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();
	//UpdateHUD();
	StartLevel();
	
	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
	);
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			FString CurrentMapName = GetWorld()->GetMapName();
			if (CurrentMapName.Contains("MenuLevel"))
			{
				SpartaPlayerController->ShowMainMenu(false);
			}
		}
	}
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstace = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstace);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::StartLevel()
{

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
				SpartaPlayerController->ShowGameHUD();
				SpartaPlayerController->GetWorldTimerManager().SetTimer(
					LevelTimerHandle,
					this,
					&ASpartaGameState::OnLevelTimeUp,
					LevelDuration,
					false
				);
		}
	}

	if (UGameInstance* GameInstace = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstace);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	int32 BaseSpawnCount = 40;
	float SpawnIncreaseRate = 0.2f;

	ItemToSpawn = BaseSpawnCount + (BaseSpawnCount * SpawnIncreaseRate * CurrentLevelIndex);

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				//IsA 하위 class도 고려함
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Level %d Start!, Spawned %d coin"),
		CurrentLevelIndex + 1,
		SpawnedCoinCount);
}

void ASpartaGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected %d / %d"), 
		CollectedCoinCount, 
		SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndLevel();
	}
}

void ASpartaGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	if (UGameInstance* GameInstace = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstace);
		if (SpartaGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;

			if (CurrentLevelIndex >= MaxLevels)
			{
				OnGameOver();
				return;
			}
			if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
			{
				GetWaveLevel();
				//UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
			}
			else
			{
				OnGameOver();
			}
		}
	}

}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowEndMenu();
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Game Over!!"));
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast< UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time : %.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score : %d "), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* HPText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("HP"))))
				{
					// PlayerController에서 Pawn 가져오기
					if (ASpartaCharacter* SpartaCharacter = Cast<ASpartaCharacter>(SpartaPlayerController->GetPawn()))
					{
						float CurrentHP = SpartaCharacter->GetHealth();

						HPText->SetText(FText::FromString(
							FString::Printf(TEXT("HP : %.0f / 100"), CurrentHP)
						));
					}
				}

			}
			if (UUserWidget* WaveWidget = SpartaPlayerController->GetWaveWidget())
			{
				if (UTextBlock* LevelIndexText = Cast< UTextBlock>(WaveWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d"), CurrentLevelIndex + 1)));
				}
			}
		}
	}
}

void ASpartaGameState::GetWaveLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
			{
				if (SpartaGameInstance->CurrentLevelIndex >= MaxLevels)
				{
					SpartaGameInstance->CurrentLevelIndex = 0;
					SpartaGameInstance->TotalScore = 0;
					CurrentLevelIndex = 0;

					SpartaPlayerController->ShowWaveHUD();

					FTimerHandle RestartHandle;
					GetWorldTimerManager().SetTimer(
						RestartHandle,
						[this]()
						{
							UE_LOG(LogTemp, Warning, TEXT("Restarting game: Opening BasicLevel"));
							UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
						},
						5.0f,
						false
					);
					return;
				}
			}

			if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
			{
				SpartaPlayerController->ShowWaveHUD();

				FTimerHandle LoadLevelHandle;
				GetWorldTimerManager().SetTimer(
					LoadLevelHandle,
					this,
					&ASpartaGameState::StartGameFunction,
					5.0f,
					false
				);
			}

		}
	}
}

void ASpartaGameState::StartGameFunction()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			//UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
			GetNextLevel();

			GetWorldTimerManager().SetTimer(
				StartLevelHandle,
				this,
				&ASpartaGameState::StartLevel,
				1.0f,
				false
			);
		}
	}
}

void ASpartaGameState::GetNextLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
}
