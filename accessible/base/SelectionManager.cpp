 // selection.
  Selection* spellSel = frameSel->GetSelection(SelectionType::eSpellCheck);
  spellSel->RemoveSelectionListener(this);

  if (mCurrCtrlNormalSel) {
    if (mCurrCtrlNormalSel->GetPresShell() == aPresShell) {
      // Remove 'this' registered as selection listener for the normal selection
      // if we are removing listeners for its PresShell.
      mCurrCtrlNormalSel->RemoveSelectionListener(this);
      mCurrCtrlNormalSel = nullptr;
    }
  }

  if (mCurrCtrlSpellSel) {
    if (mCurrCtrlSpellSel->GetPresShell() == aPresShell) {
      // Remove 'this' registered as selection listener for the spellcheck
      // selection if we are removing listeners for its PresShell.
      mCurrCtrlSpellSel->RemoveSelectionListener(this);
      mCurrCtrlSpellSel = nullptr;
    }
  }
}

void
