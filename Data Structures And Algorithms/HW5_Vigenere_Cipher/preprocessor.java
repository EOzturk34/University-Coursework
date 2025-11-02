public class preprocessor {
	private String initial_string;
	private String preprocessed_string;
	
	// initial_string is initialized as str
	public preprocessor(String str) {
		initial_string = str;
	}

	public void preprocess() {
		// do not edit this method
		capitalize();
		clean();
	}
	// If there is an 'ı' character in the string, it must be removed from the string before converting it to a capital 'I' character, 
	// since it is not an English character. The characters are then converted to uppercase. 
	// Since lowercase 'i' characters are converted to uppercase 'İ' characters, they are converted to capital 'I' by making an edit.
	private void capitalize() {
		initial_string  = initial_string .replaceAll("ı", ""); // Not in English alphabet
		initial_string  = initial_string .replaceAll("İ", ""); // Not in English alphabet
		initial_string  = initial_string .toUpperCase();
		initial_string  = initial_string .replace('İ', 'I');
		preprocessed_string = initial_string;
	}
   // String is cleared of characters not from the English alphabet
	private void clean() {
		initial_string  = initial_string .replaceAll("[^A-Z]", "");
		preprocessed_string = initial_string;
	}
	// returns preprocessed_string
	public String get_preprocessed_string() {
		return preprocessed_string;
	}
}