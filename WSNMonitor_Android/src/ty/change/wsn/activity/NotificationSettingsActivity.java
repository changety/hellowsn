package ty.change.wsn.activity;

import ty.change.wsn.androidpn.client.LogUtil;
import ty.change.wsn.util.Constants;
import android.content.Context;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.util.Log;

/**
 * @author change_ty
 *
 */
public class NotificationSettingsActivity extends PreferenceActivity {

    private static final String LOGTAG = LogUtil
            .makeLogTag(NotificationSettingsActivity.class);

    public NotificationSettingsActivity() {
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setPreferenceScreen(createPreferenceHierarchy());
        setPreferenceDependencies();

        CheckBoxPreference notifyPref = (CheckBoxPreference) getPreferenceManager()
                .findPreference(Constants.SETTINGS_NOTIFICATION_ENABLED);
        if (notifyPref.isChecked()) {
            notifyPref.setTitle("Notifications Enabled");
        } else {
            notifyPref.setTitle("Notifications Disabled");
        }
    }

    private PreferenceScreen createPreferenceHierarchy() {
        Log.d(LOGTAG, "createSettingsPreferenceScreen()...");

        PreferenceManager preferenceManager = getPreferenceManager();
        preferenceManager
                .setSharedPreferencesName(Constants.SHARED_PREFERENCE_NAME);
        preferenceManager.setSharedPreferencesMode(Context.MODE_PRIVATE);

        PreferenceScreen root = preferenceManager.createPreferenceScreen(this);

        CheckBoxPreference notifyPref = new CheckBoxPreference(this);
        notifyPref.setKey(Constants.SETTINGS_NOTIFICATION_ENABLED);
	      notifyPref.setTitle("开启推送");
	      notifyPref.setSummaryOn("启动推送服务");
	      notifyPref.setSummaryOff("暂停推送服务");
        notifyPref.setDefaultValue(Boolean.TRUE);
        notifyPref
                .setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
                    public boolean onPreferenceChange(Preference preference,
                            Object newValue) {
                        boolean checked = Boolean.valueOf(newValue.toString());
                        if (checked) {
                            preference.setTitle("开启推送");
                        } else {
                            preference.setTitle("关闭推送");
                        }
                        return true;
                    }
                });

        CheckBoxPreference soundPref = new CheckBoxPreference(this);
        soundPref.setKey(Constants.SETTINGS_SOUND_ENABLED);
        soundPref.setTitle("声音提示");
        soundPref.setSummary("推送消息时声音提示");
        soundPref.setDefaultValue(Boolean.TRUE);

        CheckBoxPreference vibratePref = new CheckBoxPreference(this);
        vibratePref.setKey(Constants.SETTINGS_VIBRATE_ENABLED);
        vibratePref.setTitle("震动提示");
        vibratePref.setSummary("推送消息时手机震动提示");
        vibratePref.setDefaultValue(Boolean.TRUE);

        root.addPreference(notifyPref);
        root.addPreference(soundPref);
        root.addPreference(vibratePref);


        return root;
    }

    private void setPreferenceDependencies() {
        Preference soundPref = getPreferenceManager().findPreference(
                Constants.SETTINGS_SOUND_ENABLED);
        if (soundPref != null) {
            soundPref.setDependency(Constants.SETTINGS_NOTIFICATION_ENABLED);
        }
        Preference vibratePref = getPreferenceManager().findPreference(
                Constants.SETTINGS_VIBRATE_ENABLED);
        if (vibratePref != null) {
            vibratePref.setDependency(Constants.SETTINGS_NOTIFICATION_ENABLED);
        }
    }

}
