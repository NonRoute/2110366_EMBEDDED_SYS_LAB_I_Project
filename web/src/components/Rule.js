import * as React from 'react';
import Box from '@mui/material/Box';
import Button from '@mui/material/Button';
import Typography from '@mui/material/Typography';
import Modal from '@mui/material/Modal';

const style = {
	position: 'absolute',
	top: '50%',
	left: '50%',
	transform: 'translate(-50%, -50%)',
	width: 400,
	bgcolor: 'background.paper',
	border: '2px solid #000',
	boxShadow: 24,
	p: 4,
	wordWrap: 'break-word',
};

export default function Rule() {
	const [open, setOpen] = React.useState(false);
	const handleOpen = () => setOpen(true);
	const handleClose = () => setOpen(false);

	return (
		<div>
			<Button onClick={handleOpen} sx={{ color: 'white', p: 0 }}>
				Rule
			</Button>
			<Modal
				open={open}
				onClose={handleClose}
				aria-labelledby="modal-modal-title"
				aria-describedby="modal-modal-description"
			>
				<Box sx={style}>
					<Typography
						id="modal-modal-title"
						variant="h6"
						component="h2"
					>
						Rule
					</Typography>
					<Typography
						id="modal-modal-description"
						sx={{ mt: 2, textIndent: '10%' }}
					>
						At the start of the game, there will be a total
						countdown of 2 minutes, after which the website will
						select one color at random. Players must find any
						objects that have the same color as the color produced
						by the website. When the items are discovered, the
						players will place them in front of the color sensor and
						clap three times to tell the website to verify the
						colors. If the color of the chosen object matches the
						color displayed by the website, the player will receive
						a point. The website will then randomly select a new
						color and continue to gain points. However, if the color
						of the item is not similar, the player must check
						another item. Players must try as many times as possible
						to find items with colors that are similar to the color
						chosen by the website. to collect as many points as
						possible within the time limit
					</Typography>
				</Box>
			</Modal>
		</div>
	);
}
