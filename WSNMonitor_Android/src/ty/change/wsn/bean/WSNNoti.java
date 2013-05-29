package ty.change.wsn.bean;

import java.io.Serializable;

import com.j256.ormlite.field.DatabaseField;
import com.j256.ormlite.table.DatabaseTable;

@DatabaseTable(tableName = "WSN_Notification")
public class WSNNoti implements Serializable {
	private static final long serialVersionUID = -809290842000488077L;

	@DatabaseField(generatedId = true)
	private int id;
	@DatabaseField
	private String title;
	@DatabaseField
	private String message;
	@DatabaseField
	private String uri;
	@DatabaseField
	private String time;

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public String getMessage() {
		return message;
	}

	public void setMessage(String message) {
		this.message = message;
	}

	public String getUri() {
		return uri;
	}

	public void setUri(String url) {
		this.uri = url;
	}

	public String getTime() {
		return time;
	}

	public void setTime(String time) {
		this.time = time;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}
}
