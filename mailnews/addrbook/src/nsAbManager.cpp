@@ -544,7 +544,7 @@ NS_IMETHODIMP nsAbManager::ExportAddressBook(mozIDOMWindowProxy *aParentWin, nsI
                                    ArrayLength(formatStrings), getter_Copies(title));
  NS_ENSURE_SUCCESS(rv, rv);
  rv = filePicker->Init(aParentWin, title, nsIFilePicker::modeSave, false);
  NS_ENSURE_SUCCESS(rv, rv);

  filePicker->SetDefaultString(dirName);
