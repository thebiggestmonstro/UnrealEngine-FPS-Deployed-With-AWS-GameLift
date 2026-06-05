// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/SignIn/SignUpPage.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void USignUpPage::NativeConstruct()
{
	Super::NativeConstruct();

	TextBox_UserName->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateSignUpButtonState);
	TextBox_Password->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateSignUpButtonState);
	TextBox_ConfirmPassword->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateSignUpButtonState);
	TextBox_Email->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateSignUpButtonState);
	Button_SignUp->SetIsEnabled(false);
}

void USignUpPage::UpdateSignUpButtonState(const FText& Text)
{
	const FString Username = TextBox_UserName->GetText().ToString();
	const FString Password = TextBox_Password->GetText().ToString();
	const FString ConfirmPassword = TextBox_ConfirmPassword->GetText().ToString();
	const FString Email = TextBox_Email->GetText().ToString();

	FString ErrorMessage;
	bool bIsValid = false;

	if (Username.IsEmpty())
	{
		ErrorMessage = TEXT("Please enter a valid Username.");
	}
	else if (Email.IsEmpty() || !IsValidEmail(Email))
	{
		ErrorMessage = TEXT("Please enter a valid email.");
	}
	else if (Password.Len() < 8) 
	{
		ErrorMessage = TEXT("Passwords must be at least 8 characters.");
	}
	else if (Password != ConfirmPassword)
	{
		ErrorMessage = TEXT("Please ensure that passwords match.");
	}
	else
	{
		bIsValid = true;
	}

	TextBlock_StatusMessage->SetText(FText::FromString(ErrorMessage));
	Button_SignUp->SetIsEnabled(bIsValid);
}

bool USignUpPage::IsValidEmail(const FString& Email)
{
	static const FRegexPattern EmailPattern(TEXT(R"((^[^\s@]+@[^\s@]+\.[^\s@]{2,}$))"));
	FRegexMatcher Matcher(EmailPattern, Email);
	return Matcher.FindNext();
}

bool USignUpPage::IsStrongPassword(const FString& Password, FString& StatusMessage)
{
	static const FRegexPattern NumberPattern(TEXT(R"(\d)"));
	static const FRegexPattern SpecialCharPattern(TEXT(R"([^\w\s])"));
	static const FRegexPattern UppercasePattern(TEXT(R"([A-Z])"));
	static const FRegexPattern LowercasePattern(TEXT(R"([a-z])"));

	FRegexMatcher NumberMatcher(NumberPattern, Password);
	if (!NumberMatcher.FindNext())
	{
		StatusMessage = TEXT("Password must contain at least 1 number.");
		return false;
	}

	FRegexMatcher SpecialCharMatcher(SpecialCharPattern, Password);
	if (!SpecialCharMatcher.FindNext())
	{
		StatusMessage = TEXT("Password must contain at least one special character.");
		return false;
	}

	FRegexMatcher UppercaseMatcher(UppercasePattern, Password);
	if (!UppercaseMatcher.FindNext())
	{
		StatusMessage = TEXT("Password must contain at least one uppercase character.");
		return false;
	}

	FRegexMatcher LowercaseMatcher(LowercasePattern, Password);
	if (!LowercaseMatcher.FindNext())
	{
		StatusMessage = TEXT("Password must contain at least one lowercase character.");
		return false;
	}

	return true;
}



