// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FItemSpawnRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ITemClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Spawnchance;

	//TSubclassOf - 하드 레퍼런스
	//클래스가 항상 메모리에 로드된 형태에서 바로 접근
	//TSoftClassPtr - 소프트 레퍼런스
	//클래스의 경로만 유지, 후에 사용할 때 로드를 시작
};
