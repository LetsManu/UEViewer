#ifndef __UE4_AES_KEY_DIALOG_H__
#define __UE4_AES_KEY_DIALOG_H__

#if UNREAL4

class UIUE4AesKeyDialog : public UIBaseDialog
{
public:
	FString Show()
	{
		if (!ShowModal("Please enter AES encryption key", -1, -1))
			return "";

		// Save typed encryption key
		return Value;
	}

	void InitUI()
	{
		(*this)
		[
			NewControl(UIGroup, GROUP_NO_BORDER|GROUP_HORIZONTAL_LAYOUT)
			.SetWidth(EncodeWidth(1.0f))
			[
				NewControl(UIBitmap)
				.SetWidth(48)
				.SetHeight(48)
				.SetResourceIcon(UIBitmap::BI_Warning)
				+ NewControl(UISpacer, 8)
				+NewControl(UIGroup, GROUP_NO_BORDER)
				.SetWidth(EncodeWidth(1.0f))
				[
					NewControl(UILabel, "UE Viewer has found an encrypted UE4 pak file. In order to").SetAutoSize()
					+NewControl(UILabel, "work correctly please specify an AES encryption key which").SetAutoSize()
					+NewControl(UILabel, "is used for this game.").SetAutoSize()
					+NewControl(UISpacer)
					+NewControl(UITextEdit, &Value)
					+NewControl(UISpacer)
					+NewControl(UIGroup, GROUP_NO_BORDER|GROUP_HORIZONTAL_LAYOUT)
					[
						NewControl(UISpacer, -1)
						+NewControl(UIButton, "Ok")
						.SetWidth(80)
						.SetOK()
//						.Enable(false)
//						.Expose(OkButton)
						+NewControl(UIButton, "Cancel")
						.SetWidth(80)
						.SetCancel()
					]
				]
			]
		];
	}

	FString		Value;
//	UIButton*	OkButton;
};

#endif // UNREAL4

#endif // __UE4_AES_KEY_DIALOG_H__
