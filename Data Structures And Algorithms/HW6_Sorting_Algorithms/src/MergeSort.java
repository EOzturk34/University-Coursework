public class MergeSort extends SortAlgorithm {
	
	public MergeSort(int input_array[]) {
		super(input_array);
	}
	
	private void merge(int arr[], int left, int mid, int right){
        int n1 = mid - left + 1; // calculate size for left array
        int n2 = right - mid; // calculate size for right array

        int leftArray[] = new int[n1]; // create temp array for left array
        int rightArray[] = new int[n2]; // create temp array right array

        for(int i = 0; i < n1; ++i){
            leftArray[i] = arr[left + i]; // fill the left array
        }
        for(int i = 0; i < n2; ++i){
            rightArray[i] = arr[mid + 1 + i]; // fill the right array
        }

        int i = 0; // temporary starting index for left array
        int j = 0; // temporary starting index for right array
        int k = left; // temporary starting index for the array
        while(i < n1 && j < n2){
            comparison_counter++; // increment counter for each comparison
            if(leftArray[i] <= rightArray[j]){ // compare 2 temporary arrays and fill the arr
                arr[k] = leftArray[i];
                i++;
            }
            else{
                arr[k] = rightArray[j];
                j++;
            }
            k++;
        }

        while(i < n1){ // fill the arr with remaining elements in left array
            arr[k] = leftArray[i];
            i++;
            k++;
        }

        while(j < n2){ // fill the arr with remaining elements in right array
            arr[k] = rightArray[j];
            j++;
            k++;
        }

    }

    private void sort(int arr[], int left, int right){
        if(left < right){
            int mid = (left + right) / 2; // calculate mid point
            sort(arr, left ,mid); // left array sorted recursively
            sort(arr, mid + 1, right); // right array sorted recursively
            merge(arr, left, mid, right); // merge the right and left array
        }
    }
    
    @Override
    public void sort() {
    	sort(arr, 0, arr.length - 1); // call the private sort() method
    }
    
    @Override
    public void print() {
    	System.out.print("Merge Sort\t=>\t");
    	super.print();
    }
}
