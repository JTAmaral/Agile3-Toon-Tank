// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameModeBase.h"
#include "ToonTanks/Pawns/PawnTank.h"
#include "ToonTanks/Pawns/PawnTurret.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanks/PlayerControllers/PlayerControllerBase.h"

void ATankGameModeBase::BeginPlay() 
{
    Super::BeginPlay();

    // Get reference and game win/lost conditions.

    // Call HandleGameStart() to initialise the start countdown, turret activation, pawn check etc.
    HandleGameStart();
}

void ATankGameModeBase::ActorDied(AActor* DeadActor) 
{
    // Check what type of Actor died. If Turret, tally. If Player -> go to lose condition.
    if(DeadActor == PlayerTank)
    {
        PlayerTank->HandleDestruction();
        HandleGameOver(false);

        if(PlayerControllerRef)
        {
            PlayerControllerRef->SetPlayerEnabledState(false);
        }
    }
    else if(APawnTurret* DestroyedTurret = Cast<APawnTurret>(DeadActor))
    {
        DestroyedTurret->HandleDestruction();

        if(--TargetTurrets == 0)
        {
            HandleGameOver(true);
        }
    }
}

void ATankGameModeBase::HandleGameStart() 
{
    // Initialise the start countdown, turret activation, pawn check etc.
    TargetTurrets = GetTargetTurretCount();
    PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
    PlayerControllerRef = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(this, 0));

    // Call blueprint version GameStart();
    GameStart();
    if(PlayerControllerRef)
    {
        PlayerControllerRef->SetPlayerEnabledState(false);

        FTimerHandle PlayerEnableHandle;
        FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject(PlayerControllerRef,
                                              &APlayerControllerBase::SetPlayerEnabledState, true);
        GetWorld()->GetTimerManager().SetTimer(PlayerEnableHandle, PlayerEnableDelegate, StartDelay, false);
    }
}

void ATankGameModeBase::HandleGameOver(bool PlayerWon) 
{
    // See if the player has destroyed all the turrets, show win result.
    // else if turret destroye playter, show lose results.
    // Call blueprint version GameOver(bool);
    GameOver(PlayerWon);
}

int32 ATankGameModeBase::GetTargetTurretCount() 
{
    TArray<AActor*> TurretActors; 
    //UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawnTurret::StaticsClass(), TurretActors);
    return TurretActors.Num();
}
