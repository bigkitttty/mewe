@@ -690,6 +690,7 @@ private:
      case CKM_AES_GCM:
        gcmParams.pIv = mIv.Elements();
        gcmParams.ulIvLen = mIv.Length();
          gcmParams.pAAD = mAad.Elements();
        gcmParams.ulAADLen = mAad.Length();
        gcmParams.ulTagBits = mTagLength;
