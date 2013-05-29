package ty.change.wsn.activity;

import ty.change.wsn.androidpn.client.LogUtil;
import ty.change.wsn.bean.WSNNoti;
import ty.change.wsn.util.Constants;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * Activity for displaying the notification details view.
 * 
 * @author Sehwan Noh (devnoh@gmail.com)
 */
public class NotificationDetailsActivity extends Activity {

	private static final String LOGTAG = LogUtil
			.makeLogTag(NotificationDetailsActivity.class);

	private String callbackActivityPackageName;

	private String callbackActivityClassName;

	private WSNNoti wsnNoti;

	public NotificationDetailsActivity() {
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		SharedPreferences sharedPrefs = this.getSharedPreferences(
				Constants.SHARED_PREFERENCE_NAME, Context.MODE_PRIVATE);
		callbackActivityPackageName = sharedPrefs.getString(
				Constants.CALLBACK_ACTIVITY_PACKAGE_NAME, "");
		callbackActivityClassName = sharedPrefs.getString(
				Constants.CALLBACK_ACTIVITY_CLASS_NAME, "");

		Intent intent = getIntent();
		wsnNoti = (WSNNoti) intent.getSerializableExtra(Constants.WSN_NOTI);
		if (wsnNoti == null) {
			String notificationId = intent
					.getStringExtra(Constants.NOTIFICATION_ID);
			String notificationApiKey = intent
					.getStringExtra(Constants.NOTIFICATION_API_KEY);
			String notificationTitle = intent
					.getStringExtra(Constants.NOTIFICATION_TITLE);
			String notificationMessage = intent
					.getStringExtra(Constants.NOTIFICATION_MESSAGE);
			String notificationUri = intent
					.getStringExtra(Constants.NOTIFICATION_URI);

			String notificationTime = intent
					.getStringExtra(Constants.NOTIFICATION_TIME);

			Log.d(LOGTAG, "notificationId=" + notificationId);
			Log.d(LOGTAG, "notificationApiKey=" + notificationApiKey);
			Log.d(LOGTAG, "notificationTitle=" + notificationTitle);
			Log.d(LOGTAG, "notificationMessage=" + notificationMessage);
			Log.d(LOGTAG, "notificationUri=" + notificationUri);

			View rootView = createView(notificationTitle, notificationMessage,
					notificationUri, notificationTime);
			setContentView(rootView);
		} else {
			String notificationTitle = wsnNoti.getTitle();
			String notificationMessage = wsnNoti.getMessage();
			String notificationUri = wsnNoti.getUri();
			String notificationTime = wsnNoti.getTime();

			Log.d(LOGTAG, "notificationTitle=" + notificationTitle);
			Log.d(LOGTAG, "notificationMessage=" + notificationMessage);

			View rootView = createView(notificationTitle, notificationMessage,
					notificationUri, notificationTime);
			setContentView(rootView);
		}
	}

	private View createView(final String title, final String message,
			final String uri, final String time) {

		LinearLayout linearLayout = new LinearLayout(this);
		linearLayout.setBackgroundColor(0xffeeeeee);
		linearLayout.setOrientation(LinearLayout.VERTICAL);
		linearLayout.setPadding(5, 5, 5, 5);
		LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
				LinearLayout.LayoutParams.FILL_PARENT,
				LinearLayout.LayoutParams.FILL_PARENT);
		linearLayout.setLayoutParams(layoutParams);

		TextView textTitle = new TextView(this);
		textTitle.setText(title);
		textTitle.setTextSize(18);
		// textTitle.setTextSize(TypedValue.COMPLEX_UNIT_SP, 18);
		textTitle.setTypeface(Typeface.DEFAULT, Typeface.BOLD);
		textTitle.setTextColor(0xff000000);
		textTitle.setGravity(Gravity.CENTER);

		layoutParams = new LinearLayout.LayoutParams(
				LinearLayout.LayoutParams.FILL_PARENT,
				LinearLayout.LayoutParams.WRAP_CONTENT);
		layoutParams.setMargins(30, 30, 30, 0);
		textTitle.setLayoutParams(layoutParams);
		linearLayout.addView(textTitle);

		TextView textDetails = new TextView(this);
		textDetails.setText(message);
		textDetails.setTextSize(14);
		textDetails.setTextColor(0xff333333);
		textDetails.setGravity(Gravity.CENTER);

		layoutParams = new LinearLayout.LayoutParams(
				LinearLayout.LayoutParams.FILL_PARENT,
				LinearLayout.LayoutParams.WRAP_CONTENT);
		layoutParams.setMargins(30, 10, 30, 20);
		textDetails.setLayoutParams(layoutParams);
		linearLayout.addView(textDetails);

		TextView textTime = new TextView(this);
		textTime.setText(time);
		textTime.setTextSize(14);
		textTime.setTextColor(0xff333333);
		textTime.setGravity(Gravity.CENTER);

		layoutParams = new LinearLayout.LayoutParams(
				LinearLayout.LayoutParams.FILL_PARENT,
				LinearLayout.LayoutParams.WRAP_CONTENT);
		layoutParams.setMargins(30, 10, 30, 20);
		textTime.setLayoutParams(layoutParams);
		linearLayout.addView(textTime);

		Button okButton = new Button(this);
		okButton.setText("确认");
		okButton.setWidth(100);

		okButton.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				if (wsnNoti == null) {
					Intent intent;
					if (uri != null
							&& uri.length() > 0
							&& (uri.startsWith("http:")
									|| uri.startsWith("https:")
									|| uri.startsWith("tel:") || uri
									.startsWith("geo:"))) {
						intent = new Intent(Intent.ACTION_VIEW, Uri.parse(uri));
					} else {
						intent = new Intent().setClassName(
								callbackActivityPackageName,
								callbackActivityClassName);
					}
					NotificationDetailsActivity.this.startActivity(intent);
					NotificationDetailsActivity.this.finish();
				} else {
					// ViewUtility.NavigateActivity(
					// NotificationDetailsActivity.this,
					// NotificationsActivity.class, true);
					NotificationDetailsActivity.this.finish();
				}
			}
		});

		LinearLayout innerLayout = new LinearLayout(this);
		innerLayout.setGravity(Gravity.CENTER);
		innerLayout.addView(okButton);

		linearLayout.addView(innerLayout);

		return linearLayout;
	}
}
