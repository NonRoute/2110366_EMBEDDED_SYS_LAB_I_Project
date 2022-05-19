import { Typography } from '@mui/material';
import React from 'react';
import { CountdownCircleTimer } from 'react-countdown-circle-timer';

export default function CountdownTimer() {
	// const [isPlaying, setIsPlaying] = React.useState(true);

	return (
		<CountdownCircleTimer
			onComplete={() => {
				console.log('done');
				return { shouldRepeat: true, delay: 2 };
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
	);
}
