package org.kawracore.qt;

import android.os.Bundle;
import android.system.ErrnoException;
import android.system.Os;

import org.qtproject.qt5.android.bindings.QtActivity;

import java.io.File;

public class KawraQtActivity extends QtActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        final File kawraDir = new File(getFilesDir().getAbsolutePath() + "/.kawra");
        if (!kawraDir.exists()) {
            kawraDir.mkdir();
        }

        super.onCreate(savedInstanceState);
    }
}
