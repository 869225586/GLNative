package com.sunyeyu.video.uikit.opengl;

import android.media.MediaCodecList;

import java.util.HashMap;
import java.util.Map;

public class VideoSupportUitl {

    private static Map<String, String> codecMap = new HashMap<>();
    static {
        codecMap.put("h264", "video/avc");
        codecMap.put("hevc","video/hevc");
        codecMap.put("mpeg4","video/mp4v-es");
        codecMap.put("wmv3","video/x-ms-wmv");
    }

    public static String findVideoCodecName(String ffcodename)
    {
        if(codecMap.containsKey(ffcodename))
        {
            return codecMap.get(ffcodename);
        }
        return "";
    }

    public static boolean isSupportCodec(String ffcodecname)
    {
        boolean supportvideo = false;
        int count = MediaCodecList.getCodecCount();
        for(int i = 0; i < count; i++)
        {
            String[] tyeps = MediaCodecList.getCodecInfoAt(i).getSupportedTypes();
            for(int j = 0; j < tyeps.length; j++)
            {
                if(tyeps[j].equals(findVideoCodecName(ffcodecname)))
                {
                    supportvideo = true;
                    break;
                }
            }
            if(supportvideo)
            {
                break;
            }
        }
        return supportvideo;
    }
}
