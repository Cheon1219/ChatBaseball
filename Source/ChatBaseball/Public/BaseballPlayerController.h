// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseballPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CHATBASEBALL_API ABaseballPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	ABaseballPlayerController();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SendMessage(const FString& Message);

	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool GetMyTurn();

	void SetMyTurn(bool NewTurn);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> WidgetClass;

	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable)
	void Client_UpdateWidget();

	UFUNCTION(Client, Reliable)
	void Client_NotifyMessage(const FString& Message);

	UFUNCTION(Client, Reliable)
	void Client_QuitGame();


private:
	
	bool bIsMyTurn = false;
};
