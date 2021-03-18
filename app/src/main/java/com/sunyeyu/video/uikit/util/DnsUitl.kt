package com.sunyeyu.video.uikit.util

import java.net.InetAddress
import java.net.UnknownHostException

class DnsUitl {
    companion object{

      //这个 host 不能带 http //  直接放host 例如 www.baidu.com
      public  fun parseHostGetIPAddress(host: String?): Array<String?>? {

            var ipAddressArr: Array<String?>? = null
            try {
                val inetAddressArr = InetAddress.getAllByName(host)
                if (inetAddressArr != null && inetAddressArr.size > 0) {
                    ipAddressArr = arrayOfNulls(inetAddressArr.size)
                    for (i in inetAddressArr.indices) {
                        ipAddressArr[i] = inetAddressArr[i].hostAddress
                    }
                }
            } catch (e: UnknownHostException) {
                e.printStackTrace()
                return null
            }
            return ipAddressArr
        }

    }

}