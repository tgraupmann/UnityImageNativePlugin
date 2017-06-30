using System;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine;

public class ImageManager : MonoBehaviour
{
    [DllImport("UnityImageNativePlugin")]
	private static extern void PluginLoadImage(IntPtr path);

	[DllImport("UnityImageNativePlugin")]
	private static extern int PluginGetFrameCount();

	[DllImport("UnityImageNativePlugin")]
	private static extern int PluginGetPixel(int frame, int x, int y);

    #region Handle Debug.Log from unmanged code

    [DllImport("UnityImageNativePlugin")]
    private static extern void PluginSetLogDelegate(IntPtr logDelegate);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void DebugLogDelegate(string text);

    private static IntPtr _sLogDelegate = IntPtr.Zero;

    private static void LogCallBack(string text)
    {
        if (string.IsNullOrEmpty(text))
        {
            return;
        }
        Debug.Log(string.Format(":C++: {0}", text));
    }

    private static void SetupLogMechanism()
    {
        DebugLogDelegate logCallback = new DebugLogDelegate(LogCallBack);
        _sLogDelegate = Marshal.GetFunctionPointerForDelegate(logCallback);

        // Call the API passing along the function pointer.
        PluginSetLogDelegate(_sLogDelegate);
    }

    #endregion

    public static void LoadImage(string path)
    {
        byte[] array = UTF8Encoding.UTF8.GetBytes(path);
        IntPtr lpData = Marshal.AllocHGlobal(array.Length);
        Marshal.Copy(array, 0, lpData, array.Length);
        PluginLoadImage(lpData);
        Marshal.FreeHGlobal(lpData);
    }

    static ImageManager()
    {
        SetupLogMechanism();

        LoadImage("Hello World!");

        int frameCount = PluginGetFrameCount();

        int color = PluginGetPixel(1, 2, 3);
    }
}
