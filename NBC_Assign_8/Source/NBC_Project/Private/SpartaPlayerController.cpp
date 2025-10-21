// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "SpartaGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

ASpartaPlayerController::ASpartaPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr),
	EndWidgetClass(nullptr),
	EndWidgetInstance(nullptr),
	WaveWidgetClass(nullptr),
	WaveWidgetInstance(nullptr)
{
}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

UUserWidget* ASpartaPlayerController::GetWaveWidget() const
{
	return WaveWidgetInstance;
}

void ASpartaPlayerController::ShowMainMenu(bool bIsRestart)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (WaveWidgetInstance)
	{
		WaveWidgetInstance->RemoveFromParent();
		WaveWidgetInstance = nullptr;
	}

	if (EndWidgetInstance)
	{
		EndWidgetInstance->RemoveFromParent();
		EndWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();
			//마우스 커서 UI
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		//if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("End"))))
		//{
		//	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
		//	SpartaGameState->OnGameOver();
		//}
	}
}

void ASpartaPlayerController::ShowEndMenu()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (WaveWidgetInstance)
	{
		WaveWidgetInstance->RemoveFromParent();
		WaveWidgetInstance = nullptr;
	}

	if (EndWidgetInstance)
	{
		EndWidgetInstance->RemoveFromParent();
		EndWidgetInstance = nullptr;
	}
	
	if (EndWidgetClass)
	{
		EndWidgetInstance = CreateWidget<UUserWidget>(this, EndWidgetClass);
		if (EndWidgetInstance)
		{
			EndWidgetInstance->AddToViewport();
			//마우스 커서 UI
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}
}

void ASpartaPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (WaveWidgetInstance)
	{
		WaveWidgetInstance->RemoveFromParent();
		WaveWidgetInstance = nullptr;
	}

	if (EndWidgetInstance)
	{
		EndWidgetInstance->RemoveFromParent();
		EndWidgetInstance = nullptr;
	}


	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
			//마우스 커서 UI
			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}
		
		ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
		if (SpartaGameState)
		{
			SpartaGameState->UpdateHUD();
		}
	}
}

void ASpartaPlayerController::ShowWaveHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (WaveWidgetInstance)
	{
		WaveWidgetInstance->RemoveFromParent();
		WaveWidgetInstance = nullptr;
	}

	if (EndWidgetInstance)
	{
		EndWidgetInstance->RemoveFromParent();
		EndWidgetInstance = nullptr;
	}

	if (WaveWidgetClass)
	{
		WaveWidgetInstance = CreateWidget<UUserWidget>(this, WaveWidgetClass);
		if (WaveWidgetInstance)
		{
			WaveWidgetInstance->AddToViewport();
			//마우스 커서 UI
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}

	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->UpdateHUD();
	}
}


void ASpartaPlayerController::StartGame()
{
	if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SpartaGameInstance->CurrentLevelIndex = 0;
		SpartaGameInstance->TotalScore = 0;
	}

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains(TEXT("BasicLevel")))
	{
		if (ASpartaGameState* SpartaGameState = GetWorld()->GetGameState<ASpartaGameState>())
		{
			SpartaGameState->StartLevel();
		}
	}
	else
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	}
}