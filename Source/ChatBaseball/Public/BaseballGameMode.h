// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseballGameMode.generated.h"

/**
 * 
 */

class ABaseballPlayerController;

UCLASS()
class CHATBASEBALL_API ABaseballGameMode : public AGameModeBase
{
	GENERATED_BODY()

	ABaseballGameMode();

public:
	
	FTimerHandle TurnTimerHandle;
	FTimerHandle RestartTimerHandle;
	
	UPROPERTY(VisibleDefaultsOnly)
	int32 MAX_PLAYER_COUNT = 2;
	
	UPROPERTY(VisibleDefaultsOnly)
	int32 CURRENT_PLAYER_COUNT = 0;
	
	UPROPERTY(VisibleDefaultsOnly)
	FString AnswerValue = "";
	
	int32 BallCount = 0;
	int32 StrikeCount = 0;
	int32 TurnCount = 1;
	
	void SetTurn(ABaseballPlayerController* NewController, bool NewTurn);
	
	virtual void PostLogin(APlayerController* NewController) override;
	
	virtual void BeginPlay() override;
	
	virtual void StartPlay() override;
	
	void InitTurn();
	
	void ChangeTurn();
	
	void ReStartLevel();
	
	bool IsValidMessage(const FString& Message);
	
	UPROPERTY()
	ABaseballPlayerController* FirstPlayerController;
	
	UPROPERTY()
	ABaseballPlayerController* SecondPlayerController;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ReceiveMessage(ABaseballPlayerController* CurrentPlayerController, const FString& Message);
};


