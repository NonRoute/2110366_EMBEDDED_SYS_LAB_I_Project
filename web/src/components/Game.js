import { Typography } from '@mui/material';
import React from 'react';
import CountdownTimer from './CountdownTimer';

export default function Game() {
	return (
		<div className="main-container">
			<div className="main">
				<Typography
					variant="h1"
					sx={{ color: 'white', fontSize: '15px', padding: '2rem' }}
				>
					FIND AN ITEM THAT IS THE SAME COLOR AS THE COLOR IN THE BOX.
				</Typography>
				<CountdownTimer />
				<Typography
					sx={{ color: 'white', fontSize: '13px', padding: '1rem' }}
				>
					FOUNT IT?
				</Typography>
				<Typography sx={{ color: 'white', fontSize: '13px' }}>
					• CLAP 3 TIMES
				</Typography>
				<Typography sx={{ color: 'white', fontSize: '13px' }}>
					• PLACE THE ITEM IN FRONT OF THE SENSOR
				</Typography>
			</div>
		</div>
	);
}
