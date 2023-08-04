//Copyright (c) 2023 Swen Willson (MagnumFalls). All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Font.h"
#include "UObject/Interface.h"
#include "I_MENUConnect.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_MENUConnect : public UInterface { GENERATED_BODY() };

class SPG_CPP_API II_MENUConnect {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void EndgameCPP();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void PlateStatsCPP(int32 Good, int32 Wrong, int32 AllPlates);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ColorWidgetCPP(UFont* Font, FLinearColor Good, FLinearColor Free, FLinearColor Wrong);

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
};
