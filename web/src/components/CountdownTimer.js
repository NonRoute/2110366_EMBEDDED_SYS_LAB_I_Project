import { Typography } from '@mui/material';
import React, { useState } from 'react';
import { CountdownCircleTimer } from 'react-countdown-circle-timer';

export default function CountdownTimer() {
	// const [isPlaying, setIsPlaying] = React.useState(true);
	const [key, setKey] = useState(0);
	return (
		<div className="timer">
			<CountdownCircleTimer
				key={key}
				onComplete={() => {
					console.log('done');
					return { shouldRepeat: true, delay: 2 };
				}}
				onUpdate={(remainingTime) => {
					console.log(remainingTime);
				}}
				isPlaying
				duration={60}
				colors="#C18FEE"
			>
				{({ remainingTime, color }) => (
					<Typography sx={{ color, fontSize: 40 }}>
						{remainingTime}
					</Typography>
				)}
			</CountdownCircleTimer>
			<button onClick={() => setKey((prevKey) => prevKey + 1)}>
				Restart Timer
			</button>
		</div>
	);
}
