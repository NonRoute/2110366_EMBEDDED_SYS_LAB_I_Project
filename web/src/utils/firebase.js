// Import the functions you need from the SDKs you need
import { initializeApp } from 'firebase/app';
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
const firebaseConfig = {
	apiKey: 'AIzaSyChA3IxstEVevYQpvPV6ddn-sGkBHZC5eg',
	authDomain: 'psa-esp.firebaseapp.com',
	databaseURL:
		'https://psa-esp-default-rtdb.asia-southeast1.firebasedatabase.app',
	projectId: 'psa-esp',
	storageBucket: 'psa-esp.appspot.com',
	messagingSenderId: '559184460637',
	appId: '1:559184460637:web:efc0b753cd8734518bc6b4',
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

export default app;
