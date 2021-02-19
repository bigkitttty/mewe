
    
    CheckedNumeric<GLuint> rowCount;
    if (compressed) {
        CheckedNumeric<GLuint> checkedBlockHeight(compressedBlockHeight);
        rowCount = (pixelsHeight + checkedBlockHeight - 1u) / checkedBlockHeight;
    } else {
        rowCount = pixelsHeight;
    }

    GLuint rowPitch = 0;
    ANGLE_TRY_RESULT(computeRowPitch(formatType, width, alignment, rowLength), rowPitch);

    CheckedNumeric<GLuint> checkedRowPitch(rowPitch);
    auto depthPitch = checkedRowPitch * rowCount;
    ANGLE_TRY_CHECKED_MATH(depthPitch);
    return depthPitch.ValueOrDie();
}
