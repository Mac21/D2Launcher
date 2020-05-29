using System;
using System.Collections;
using System.Reflection;
using System.Net;
using System.Web;

namespace D2Launcher
{
    internal static class GoogleTracker
    {
        private static Uri googleURL = new Uri("http://www.google-analytics.com/collect");
        private static String googleVersion = "1";
        private static String googleTrackingID = "UA-154069532-1";
        private static String googleClientID = Environment.OSVersion.ToString();
        internal static void trackEvent(string category, string action, string label, string value)
        {
            var hashtable = baseValues();
            hashtable.Add("t", "event");
            hashtable.Add("ec", category);
            hashtable.Add("ea", action);
            if (label != null)
            {
                hashtable.Add("el", label);
            }
            if (value != null)
            {
                hashtable.Add("ev", value);
            }
            postData(hashtable);
        }
        internal static void trackPage(string hostname, string page, string title)
        {
            var hashtable = baseValues();
            hashtable.Add("t", "pageview");
            hashtable.Add("dh", hostname);
            hashtable.Add("dp", page);
            hashtable.Add("dt", title);
            postData(hashtable);
        }
        internal static void trackTiming(Int32 val)
        {
            var hashtable = baseValues();
            hashtable.Add("t", "timing");
            hashtable.Add("utc", "mapgen");
            hashtable.Add("utv", "gen");
            hashtable.Add("utt", val);
            postData(hashtable);
        }
        internal static void trackScreen(string screen)
        {
            var hashtable = baseValues();
            hashtable.Add("t", "screenview");
            hashtable.Add("an", "shalzuthmaphack");
            hashtable.Add("av", Assembly.GetExecutingAssembly().GetName().Version.ToString());
            hashtable.Add("aid", "com.shalzuthmaphack.app");
            //hashtable.Add("aiid", "com.net.d2bot");
            hashtable.Add("cd", screen);
        }
        internal static void trackException(string description, bool fatal = false)
        {
            Hashtable hashtable = baseValues();
            hashtable.Add("t", "exception");
            hashtable.Add("exd", description);
            hashtable.Add("exf", fatal ? "1" : "0");
            postData(hashtable);
        }
        static Hashtable baseValues()
        {
            var ht = new Hashtable();
            ht.Add("v", googleVersion);
            ht.Add("tid", googleTrackingID);
            ht.Add("cid", googleClientID);
            return ht;
        }
        static bool postData(Hashtable values)
        {
            string text = "";
            foreach (var obj in values.Keys)
            {
                if (text != "")
                    text += "&";
                if (values[obj] != null)
                    text = text + obj.ToString() + "=" + HttpUtility.UrlEncode(values[obj].ToString());
            }
            using (WebClient webClient = new WebClient())
                webClient.UploadStringAsync(googleURL, "POST", text);
            return true;
        }
    }
}