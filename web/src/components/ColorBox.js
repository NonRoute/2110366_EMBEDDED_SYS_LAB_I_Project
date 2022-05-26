import { Container } from '@mui/system';
import React from 'react';

export default function ColorBox(props) {
	return (
		<Container
			sx={{
				display: 'flex',
				flexDirection: 'column',
				justifyContent: 'center',
				backgroundColor: '#D9D9D9',
				width: 300,
				height: 300,
				// marginRight: '5rem',
				marginLeft: '5rem',
				marginRight: '5rem',
			}}
		>
			<Container
				sx={{
					display: 'flex',
					flexDirection: 'column',
					justifyContent: 'center',
					backgroundColor: 'white',
					width: 250,
					height: 250,
				}}
			>
				<Container
					sx={{
						backgroundColor: props.color,
						width: 200,
						height: 200,
						borderRadius: '50%',
					}}
				></Container>
			</Container>
		</Container>
	);
}
