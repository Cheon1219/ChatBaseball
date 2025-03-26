#include "BaseballGameMode.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatBaseball/Public/BaseballGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BaseballPlayerController.h"
//
 ABaseballGameMode::ABaseballGameMode()
 {
	PlayerControllerClass = ABaseballPlayerController::StaticClass();

	UE_LOG(LogTemp, Warning, TEXT("GameMode Constructor Called"));
 }
//
void ABaseballGameMode::SetTurn(ABaseballPlayerController* NewController, bool NewTurn)
{
	if (IsValid(NewController))
	{
		NewController->SetMyTurn(NewTurn);
	
		if (NewTurn)
		{
			FString TurnMessage = NewController->GetName() + TEXT("의 턴입니다.");
	
			if (GEngine)
			{
				FirstPlayerController->Client_NotifyMessage(TurnMessage);
				SecondPlayerController->Client_NotifyMessage(TurnMessage);
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TurnMessage);
			}
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Controller 초기화 오류"));
		}
	}
}
//
 void ABaseballGameMode::PostLogin(APlayerController* NewController)
 {
	Super::PostLogin(NewController);

	CURRENT_PLAYER_COUNT++;

	if (CURRENT_PLAYER_COUNT == 1)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("P1 입장"));
		FirstPlayerController = Cast<ABaseballPlayerController>(NewController);
	}
	else if (CURRENT_PLAYER_COUNT == 2)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("P2 입장"));
		SecondPlayerController = Cast<ABaseballPlayerController>(NewController);
	}

	if (CURRENT_PLAYER_COUNT == MAX_PLAYER_COUNT)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("CURRENT Player == Max Player"));
		GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &ABaseballGameMode::InitTurn, 1.0f, false);
	}
 }

void ABaseballGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BaseballGameMode::BeginPlay"));
}

//
 void ABaseballGameMode::StartPlay()
 {
 	Super::StartPlay();

	while (!IsValidMessage(AnswerValue))
	{
		int32 RandValue = FMath::RandRange(100, 999);

		AnswerValue = TEXT("/") + FString::FromInt(RandValue);
	}


	// 임시 테스트용, 서버 클라이언트에서 정답을 출력함
	//if (GEngine)
	//{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, AnswerValue);
	//}

}

 void ABaseballGameMode::InitTurn()
 {
	int32 InitTurnRandValue = FMath::RandRange(0, 1);

	switch (InitTurnRandValue)
	{
	case 0:

		SetTurn(FirstPlayerController, true);
		break;

	case 1:
		SetTurn(SecondPlayerController, true);
		break;

	default:
		break;
	}

	//FirstPlayerController->UpdateWidget();
	//SecondPlayerController->UpdateWidget();

	//FirstPlayerController->Client_UpdateWidget();
	//SecondPlayerController->Client_UpdateWidget();
 }

 void ABaseballGameMode::ChangeTurn()
 {
	FirstPlayerController->Client_NotifyMessage(TEXT("현재 턴: ") + FString::FromInt(TurnCount));
	SecondPlayerController->Client_NotifyMessage(TEXT("현재 턴: ") + FString::FromInt(TurnCount));

		if (FirstPlayerController->GetMyTurn())
		{
			FirstPlayerController->SetMyTurn(false);
			SecondPlayerController->SetMyTurn(true);

			const FString ChangeTurnMessage = SecondPlayerController->GetName() + TEXT("의 턴입니다.");

			FirstPlayerController->Client_NotifyMessage(ChangeTurnMessage);
			SecondPlayerController->Client_NotifyMessage(ChangeTurnMessage);
		}
		else
		{
			FirstPlayerController->SetMyTurn(true);
			SecondPlayerController->SetMyTurn(false);

			const FString ChangeTurnMessage = FirstPlayerController->GetName() + TEXT("의 턴입니다.");

			FirstPlayerController->Client_NotifyMessage(ChangeTurnMessage);
			SecondPlayerController->Client_NotifyMessage(ChangeTurnMessage);
		}
 }
//
 void ABaseballGameMode::ReStartLevel()
 {
	FirstPlayerController->Client_QuitGame();
	SecondPlayerController->Client_QuitGame();
	UGameplayStatics::OpenLevel(this, FName());
}

bool ABaseballGameMode::IsValidMessage(const FString& Message)
{
	if (Message.Len() == 4 && Message[0] == '/')
	{
		FString RealNumber = Message.Mid(1);

		UE_LOG(LogTemp, Warning, TEXT("RealNumber: %s"), *RealNumber);

		TSet<TCHAR> DuplicatedNumberSet;
		for (const TCHAR& Char : RealNumber)
		{
			UE_LOG(LogTemp, Warning, TEXT("Char: %c"), Char);

			if (!FChar::IsDigit(Char))
			{
				return false;
			}

			if (DuplicatedNumberSet.Contains(Char))
			{
				return false;
			}

			DuplicatedNumberSet.Add(Char);
		}

		return true;

	}

	else
	{
		return false;
	}
 }
//
//
//
void ABaseballGameMode::Multicast_ReceiveMessage_Implementation(ABaseballPlayerController* CurrentPlayerController, const FString& Message)
{
	if (IsValidMessage(Message) == true)
	{
		const FString& FullMessage = CurrentPlayerController->GetName() + TEXT(": ") + Message;
	
		FirstPlayerController->Client_NotifyMessage(FullMessage);
		SecondPlayerController->Client_NotifyMessage(FullMessage);
	
		TArray<TCHAR> AnswerArray;
		for (const TCHAR& AnswerChar : AnswerValue)
		{
			AnswerArray.Add(AnswerChar);
		}
	
		TArray<TCHAR> ClientAnswerArray;
		for (const TCHAR& ClientAnswerChar : Message)
		{
			ClientAnswerArray.Add(ClientAnswerChar);
		}
	
		for (int i = 1; i < AnswerArray.Num(); ++i)
		{
			if (AnswerArray.Contains(ClientAnswerArray[i]))
			{
				BallCount++;
				if (AnswerArray[i] == ClientAnswerArray[i])
				{
					BallCount--;
					StrikeCount++;
				}
			}
		}
	
		if (BallCount == 0 && StrikeCount == 0)
		{
			const FString& ResultMessage = TEXT("Out!");
	
			FirstPlayerController->Client_NotifyMessage(ResultMessage);
			SecondPlayerController->Client_NotifyMessage(ResultMessage);
		}
		else if (StrikeCount == 3)
		{
			const FString& ResultMessage = TEXT("HomeRun!");
	
			FirstPlayerController->Client_NotifyMessage(ResultMessage);
			SecondPlayerController->Client_NotifyMessage(ResultMessage);
	
			FirstPlayerController->SetMyTurn(false);
			SecondPlayerController->SetMyTurn(false);
	
			GetWorldTimerManager().SetTimer(RestartTimerHandle, this, &ABaseballGameMode::ReStartLevel, 3.0f, false);
			return;
		}
		else
		{
			const FString& ResultMessage = TEXT("Ball: ") + FString::FromInt(BallCount) + TEXT(", Strike: ") + FString::FromInt(StrikeCount);
	
			FirstPlayerController->Client_NotifyMessage(ResultMessage);
			SecondPlayerController->Client_NotifyMessage(ResultMessage);
		}
	
		BallCount = 0;
		StrikeCount = 0;
		TurnCount++;
	
		if (TurnCount >= 7)
		{
			FirstPlayerController->Client_NotifyMessage(TEXT("게임 종료! 답은: ") + AnswerValue);
			SecondPlayerController->Client_NotifyMessage(TEXT("게임 종료! 답은: ") + AnswerValue);
	
			FirstPlayerController->SetMyTurn(false);
			SecondPlayerController->SetMyTurn(false);
	
			GetWorldTimerManager().SetTimer(RestartTimerHandle, this, &ABaseballGameMode::ReStartLevel, 3.0f, false);
		}
		else
		{
			GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &ABaseballGameMode::ChangeTurn, 2.0f);
		}
	}
	else
	{
		CurrentPlayerController->Client_NotifyMessage(TEXT("올바른 숫자를 입력 해 주세요."));
	}

}