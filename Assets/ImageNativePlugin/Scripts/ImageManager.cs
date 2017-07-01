using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine;

public class ImageManager : MonoBehaviour
{
#if UNITY_3 || UNITY_3_0 || UNITY_3_1 || UNITY_3_2 || UNITY_3_3 || UNITY_3_4 || UNITY_3_5
    const string DLL_NAME = "UnityImageNativePlugin3";

#else
    const string DLL_NAME = "UnityImageNativePlugin";
#endif
#if UNITY_EDITOR_WIN || UNITY_STANDALONE_WIN
    [DllImport(DLL_NAME)]
	private static extern void PluginLoadImage(IntPtr path);

    [DllImport(DLL_NAME)]
    private static extern int PluginGetFrameCount();

    [DllImport(DLL_NAME)]
    private static extern int PluginGetHeight();

    [DllImport(DLL_NAME)]
    private static extern int PluginGetWidth();

    [DllImport(DLL_NAME)]
	private static extern int PluginGetPixel(int frame, int x, int y);

#region Handle Debug.Log from unmanged code

    [DllImport(DLL_NAME)]
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
        FileInfo fi = new FileInfo(path);
        if (fi.Exists)
        {
            byte[] array = ASCIIEncoding.ASCII.GetBytes(fi.FullName);
            //array[array.Length - 1] = 0;
            IntPtr lpData = Marshal.AllocHGlobal(array.Length);
            Marshal.Copy(array, 0, lpData, array.Length);
            PluginLoadImage(lpData);
            Marshal.FreeHGlobal(lpData);
        }
    }

    static ImageManager()
    {
        SetupLogMechanism();

        string path = @"Assets\ImageNativePlugin\Textures\keyboard_image.gif";
        Debug.Log(string.Format("File exists: {0}", File.Exists(path)));

        if (File.Exists(path))
        {
            LoadImage(path);
        }

        int frameCount = PluginGetFrameCount();
        Debug.Log(string.Format("Frame count: {0}", frameCount));

        int height = PluginGetHeight();
        int width = PluginGetWidth();
        for (int index = 0; index < frameCount; ++index)
        {
            for (int i = 0; i < height; ++i)
            {
                for (int j = 0; j < width; ++j)
                {
                    int color = PluginGetPixel(index, j, i);
                }
            }
        }
    }
#endif
}
