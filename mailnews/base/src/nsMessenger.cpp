     GetString(NS_LITERAL_STRING("SaveAttachment"), saveAttachmentStr);
      filePicker->Init(mWindow,
                       saveAttachmentStr,
                       nsIFilePicker::modeSave, false);
      filePicker->SetDefaultString(path);
      filePicker->AppendFilters(nsIFilePicker::filterAll);
