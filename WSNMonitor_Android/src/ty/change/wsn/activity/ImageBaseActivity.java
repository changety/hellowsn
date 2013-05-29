package ty.change.wsn.activity;

import android.os.Bundle;

import com.nostra13.universalimageloader.core.ImageLoader;

/**
 * @author change_ty
 */
public class ImageBaseActivity extends BaseActivity {

	protected ImageLoader imageLoader = ImageLoader.getInstance();

	private boolean instanceStateSaved;

	@Override
	public void onSaveInstanceState(Bundle outState) {
		instanceStateSaved = true;
	}

	@Override
	protected void onDestroy() {
		if (!instanceStateSaved) {
			imageLoader.stop();
		}
		super.onDestroy();
	}

	@Override
	protected void initView() {
		// TODO Auto-generated method stub
	}
	@Override
	protected void initData() {
		// TODO Auto-generated method stub
		
	}
	
}
