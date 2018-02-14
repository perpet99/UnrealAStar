// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AStarTestGameMode.h"
#include "AStarTestPlayerController.h"
#include "AStarTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAStarTestGameMode::AAStarTestGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AAStarTestPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}