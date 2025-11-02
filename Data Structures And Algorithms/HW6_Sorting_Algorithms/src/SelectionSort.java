public class SelectionSort extends SortAlgorithm {

	public SelectionSort(int input_array[]) {
		super(input_array);
	}

    @Override
    public void sort() {
        for(int i = 0; i < arr.length - 1; ++i){ // find the index of smallest element of remaining array each time in array 
            int minIndex = i;
            for(int j = i + 1; j < arr.length; ++j){
                comparison_counter++; // increment counter for each comparison
                if(arr[j] <= arr[minIndex]){ // find smallest element's index
                    minIndex = j;
                }
            }
            swap(minIndex, i); // place smallest element to starting position of remaining elements
        }
    }

    @Override
    public void print() {
    	System.out.print("Selection Sort\t=>\t");
    	super.print();
    }
}
