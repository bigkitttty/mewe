     GetString(NS_LITERAL_STRING("SaveAttachment"), saveAttachmentStr);
      filePicker->Init(mWindow,
                       saveAttachmentStr,
                       nsIFilePicker::modeSave, false);
      filePicker->SetDefaultString(path);
      filePicker->AppendFilters(nsIFilePicker::filterAll);
  GetString(NS_LITERAL_STRING("SaveAllAttachments"), saveAttachmentStr);
  filePicker->Init(mWindow,
                   saveAttachmentStr,
                   nsIFilePicker::modeGetFolder, false);

  rv = GetLastSaveDirectory(getter_AddRefs(lastSaveDir));
  if (NS_SUCCEEDED(rv) && lastSaveDir)
         nsString chooseFolderStr;
  GetString(NS_LITERAL_STRING("ChooseFolder"), chooseFolderStr);
  filePicker->Init(mWindow, chooseFolderStr, nsIFilePicker::modeGetFolder, false);

  nsCOMPtr<nsIFile> lastSaveDir;
  rv = GetLastSaveDirectory(getter_AddRefs(lastSaveDir));


