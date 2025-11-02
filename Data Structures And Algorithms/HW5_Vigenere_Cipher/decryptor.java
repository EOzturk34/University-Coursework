import java.util.Map;
import java.util.Iterator;

public class decryptor {
	private Map<Character, Map<Character, Character>> map;
	private String key;
	private String keystream = "";
	private String plain_text = "";
	private String cipher_text;
	// Fields initialized as follow
	public decryptor(Map<Character, Map<Character, Character>> _map, String _key, String text) {
		map = _map;
		key = _key;
		cipher_text = text;
	}

	public void decrypt() {
		// do not edit this method
		generate_keystream();
		generate_plain_text();
	}
	//A keystream is created depending on the length of the cipher_text. 
	//Characters of the key are added to the keystreamBuilder over and over again depending on the situation using circular indexing
	private void generate_keystream() {
		StringBuilder keystreamBuilder = new StringBuilder();
		int keyLength = key.length();
		int cipher_textLength = cipher_text.length();

		for(int i = 0; i < cipher_textLength; i++) {
			keystreamBuilder.append(key.charAt(i % keyLength));
		}

		keystream = keystreamBuilder.toString();
	}
	
	// plain_text is created using the map according to the characters of cipher_text and keystream.
	// map.get(x).keySet() used for an iterator
	private void generate_plain_text() {
		StringBuilder plainBuilder = new StringBuilder();
	
		for (int i = 0; i < cipher_text.length(); i++) {
			char cChar = cipher_text.charAt(i);
			char kChar = keystream.charAt(i);
	
			// Founds to inner map
			Map<Character, Character> innerMap = map.get(kChar);
	
			// Creates iterator with map.get(x).keySet
			Iterator<Character> iterator = map.get(kChar).keySet().iterator();
	
			while (iterator.hasNext()) {
				char possibleChar = iterator.next();
				if (innerMap.get(possibleChar) != null && innerMap.get(possibleChar) == cChar) {
					plainBuilder.append(possibleChar);
					break;
				}
			}
		}
	
		plain_text = plainBuilder.toString();
	}
	
    // returns keystream
	public String get_keystream() {
		return keystream;
	}
	// returns plain_text
	public String get_plain_text() {
		return  plain_text;
	}
}
