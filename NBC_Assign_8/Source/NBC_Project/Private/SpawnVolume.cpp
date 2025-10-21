// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	//Box Component�� ũ�� �������� ��
	//(200, 100, 50) (2, 1, 1) -> (400, 100, 50)
	//�߽ɿ��� ���κб��� �Ÿ� = ����
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	//�߽���ǥ
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	//������ ��ǥ���� ����
	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		//TSubclass
		if (UClass* ActualClass = SelectedRow->ITemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}
	return nullptr;
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if(!ItemDataTable) return nullptr;

	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));
	ItemDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->Spawnchance;
		}
	}

	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->Spawnchance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}

	return nullptr;
	// 0.0 ~ 1.0
	//������ 0.7
	// Item A 0.5
	// Item B 0.8
	// Item C 1.0
	//���� Ȯ���� �� �ؼ� �� �� �ȿ��� ������
	// ���� ������ ���� �� ���� ���� ���ɼ��� ����
	// ���� ���� ��
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator
	);
	return SpawnedActor;
}

