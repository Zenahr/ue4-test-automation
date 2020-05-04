#include "DaeTestAssertBlueprintFunctionLibrary.h"
#include "DaeTestActor.h"
#include "DaeTestLogCategory.h"
#include "DaeTestTriggerBox.h"
#include <Kismet/KismetMathLibrary.h>

const FString UDaeTestAssertBlueprintFunctionLibrary::ErrorMessageFormat =
    TEXT("Assertion failed - {0} - Expected: {1}, but was: {2}");

void UDaeTestAssertBlueprintFunctionLibrary::AssertFail(UObject* Context, const FString& What)
{
    OnTestFailed(Context, What);
}

void UDaeTestAssertBlueprintFunctionLibrary::AssertTrue(UObject* Context, const FString& What,
                                                        bool bValue)
{
    if (!bValue)
    {
        FString Message = FString::Format(*ErrorMessageFormat, {What, TEXT("True"), TEXT("False")});
        OnTestFailed(Context, Message);
    }
}

void UDaeTestAssertBlueprintFunctionLibrary::AssertFalse(UObject* Context, const FString& What,
                                                         bool bValue)
{
    if (bValue)
    {
        FString Message = FString::Format(*ErrorMessageFormat, {What, TEXT("False"), TEXT("True")});
        OnTestFailed(Context, Message);
    }
}

void UDaeTestAssertBlueprintFunctionLibrary::AssertWasTriggered(UObject* Context,
                                                                ADaeTestTriggerBox* TestTriggerBox)
{
    if (!IsValid(TestTriggerBox))
    {
        OnTestFailed(Context, TEXT("Invalid test trigger box in assertion"));
        return;
    }

    if (!TestTriggerBox->WasTriggered())
    {
        FString Message =
            FString::Format(TEXT("Assertion failed - Trigger box {0} wasn't triggered"),
                            {TestTriggerBox->GetName()});
        OnTestFailed(Context, Message);
        return;
    }
}

void UDaeTestAssertBlueprintFunctionLibrary::AssertEqualsVector(UObject* Context,
                                                                const FString& What,
                                                                const FVector& Actual,
                                                                const FVector& Expected)
{
    if (!Actual.Equals(Expected))
    {
        FString Message =
            FString::Format(*ErrorMessageFormat, {What, Expected.ToString(), Actual.ToString()});
        OnTestFailed(Context, Message);
    }
}

void UDaeTestAssertBlueprintFunctionLibrary::AssertInRangeFloat(UObject* Context,
                                                                const FString& What, float Value,
                                                                float MinInclusive,
                                                                float MaxInclusive)
{
    if (!UKismetMathLibrary::InRange_FloatFloat(Value, MinInclusive, MaxInclusive))
    {
        FString Message = FString::Format(
            TEXT("Assertion failed - {0} - Expected: between {1} and {2}, but was: {3}"),
            {What, MinInclusive, MaxInclusive, Value});
        OnTestFailed(Context, Message);
    }
}

void UDaeTestAssertBlueprintFunctionLibrary::OnTestFailed(UObject* Context, const FString& Message)
{
    ADaeTestActor* TestActor = Cast<ADaeTestActor>(Context);

    if (IsValid(TestActor))
    {
        TestActor->NotifyOnTestFailed(Message);
    }
    else
    {
        UE_LOG(LogDaeTest, Error, TEXT("%s"), *Message);
    }
}