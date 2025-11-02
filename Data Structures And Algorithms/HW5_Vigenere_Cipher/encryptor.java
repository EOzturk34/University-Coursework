import java.util.Map;

public class encryptor {
	private Map<Character, Map<Character, Character>> map;
	private String key;
	private String keystream = "";
	private String plain_text;
	private String cipher_text = "";
	// Field initializes as follow
	public encryptor(Map<Character, Map<Character, Character>> _map, String _key, String text) {
		map = _map;
		key = _key;
		plain_text = text;
	}
	
	public void encrypt() {
		// do not edit this method
		generate_keystream();
		generate_cipher_text();
	}
	//A keystream is created depending on the length of the plain_text. 
	//Characters of the key are added to the keystreamBuilder over and over again depending on the situation using circular indexing
	private void generate_keystream() {
		StringBuilder keystreamBuilder = new StringBuilder();
		int keyLength = key.length();
		int plain_textLength = plain_text.length();
		
		for(int i = 0; i < plain_textLength; i++){
			keystreamBuilder.append(key.charAt(i % keyLength));
		}
		keystream = keystreamBuilder.toString();
	}
	// cipher_text is created using the map according to the characters of plain text and keystream.
	private void generate_cipher_text() {
		StringBuilder cipherBuilder = new StringBuilder();
		for(int i = 0; i < plain_text.length(); i++){
			char pChar = plain_text.charAt(i); //first key
			char kChar = keystream.charAt(i); // second key

			Map<Character, Character> innerMap = map.get(pChar);
			char value = innerMap.get(kChar); // finding value according to keys
			
			cipherBuilder.append(value);
		}

		cipher_text = cipherBuilder.toString();
	}
    // returns keystream
	public String get_keystream() {
		return keystream;
	}
	// returns cipher_text
	public String get_cipher_text() {
		return cipher_text;
	}
}
