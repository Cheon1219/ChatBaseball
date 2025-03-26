// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatBaseball/Public/BaseballPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "BaseballGameMode.h"

void ABaseballPlayerController::Server_SendMessage_Implementation(const FString& Message)
{
	if (GetMyTurn() == true)
	{
		if (IsValid(GetWorld()))
		{
			if (ABaseballGameMode* GameMode = Cast<ABaseballGameMode>(GetWorld()->GetAuthGameMode()))
			{
				GameMode->Multicast_ReceiveMessage(this, Message);
			}
		}
	}
	else
	{
		Client_NotifyMessage(TEXT("당신의 턴이 아닙니다."));
	}
}

ABaseballPlayerController::ABaseballPlayerController()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller Constructor Called"));
}

bool ABaseballPlayerController::GetMyTurn()
{
	return bIsMyTurn;
}

void ABaseballPlayerController::SetMyTurn(bool NewTurn)
{
	bIsMyTurn = NewTurn;
}

void ABaseballPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("ABaseballPlayerController::BeginPlay"));

	if (WidgetClass)
	{
		UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
		if (WidgetInstance)
		{
			WidgetInstance->AddToViewport();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("위젯 없음"));
	}
}

void ABaseballPlayerController::Client_QuitGame_Implementation()
{
	ABaseballPlayerController* PlayerController = this;

	PlayerController->ClientReturnToMainMenuWithTextReason(FText::GetEmpty());
}

void ABaseballPlayerController::Client_NotifyMessage_Implementation(const FString& Message)
{
	if (IsLocalController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, Message);
	}
}

void ABaseballPlayerController::Client_UpdateWidget_Implementation()
{
	if (IsLocalController())
	{

		if (WidgetClass)
		{
			UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
			if (WidgetInstance)
			{
				WidgetInstance->AddToViewport();
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("위젯 없음"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("서버에서 위젯 생성 막음"));
	}
}