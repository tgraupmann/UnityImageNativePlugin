using System.IO;
using UnityEngine;

public class ShowTexturePixels : MonoBehaviour
{
    public Texture2D _mTexture = null;

    // Use this for initialization
    private void Start()
    {
        string path = @"Assets\ImageNativePlugin\Textures\keyboard_image.jpg";
        Debug.Log(string.Format("File exists: {0}", File.Exists(path)));

        if (File.Exists(path))
        {
            ImageManager.LoadImage(path);
        }

        int frameCount = ImageManager.PluginGetFrameCount();
        Debug.Log(string.Format("Frame count: {0}", frameCount));

        int height = ImageManager.PluginGetHeight();
        int width = ImageManager.PluginGetWidth();

        if (width > 0 &&
            height > 0)
        {
            _mTexture = new Texture2D(width, height, TextureFormat.ARGB32, false);
        }

        Color[] pixels = _mTexture.GetPixels();
        if (frameCount > 0)
        {
            for (int i = 0; i < height; ++i)
            {
                for (int j = 0; j < width; ++j)
                {
                    int bgrInt = ImageManager.PluginGetPixel(0, i, j);
                    Color color = ImageManager.ToRGB(bgrInt);
                    pixels[0] = color;
                }
            }
            _mTexture.SetPixels(pixels);
            _mTexture.Apply();
        }
    }

    private void OnGUI()
    {
        if (_mTexture)
        {
            GUILayout.Label(_mTexture, GUILayout.Width(512), GUILayout.Height(512));
        }
    }
}
